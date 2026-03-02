// Include Apple frameworks ONLY — no JUCE headers to avoid 'Component' / OpenGL conflicts.
#import <AVFoundation/AVFoundation.h>
#import <CoreVideo/CoreVideo.h>

#include "VideoBackground.h"
#include <vector>
#include <mutex>

// ── Obj-C looper ──────────────────────────────────────────────────────────────
@interface VBLooper : NSObject
@property (nonatomic, assign) AVPlayer* player;
@property (nonatomic, assign) double loopStartSec;
@end

@implementation VBLooper
- (void) itemDidFinish: (NSNotification*) note
{
    [_player seekToTime: CMTimeMakeWithSeconds(_loopStartSec, 600) toleranceBefore: kCMTimeZero
        toleranceAfter: kCMTimeZero];
    [_player play];
}
@end

// ── Pimpl ─────────────────────────────────────────────────────────────────────
struct VideoBackground::Pimpl
{
    AVPlayer*                player       = nil;
    AVPlayerItemVideoOutput* output       = nil;
    VBLooper*                looper       = nil;
    id                       timeObserver = nil;

    double loopStartSec = 0.0;
    double loopEndSec   = -1.0;   // -1 means "use end of media"

    // Frame cache — written from any thread, read from message thread.
    std::mutex           mtx;
    std::vector<uint8_t> pixels;
    int                  frameW  = 0;
    int                  frameH  = 0;
    int                  frameStride = 0;

    Pimpl() {}

    ~Pimpl()
    {
        teardown();
    }

    void play()
    {
        if (player) [player play];
    }

    void pause()
    {
        if (player) [player pause];
    }

    void stop()
    {
        if (! player) return;
        [player pause];
        [player seekToTime: kCMTimeZero
           toleranceBefore: kCMTimeZero
            toleranceAfter: kCMTimeZero];
    }

    void teardown()
    {
        if (timeObserver && player)
        {
            [player removeTimeObserver: timeObserver];
            timeObserver = nil;
        }
        if (looper && player)
        {
            [[NSNotificationCenter defaultCenter] removeObserver: looper
                                                            name: AVPlayerItemDidPlayToEndTimeNotification
                                                          object: player.currentItem];
        }
        [player pause];
        player = nil;
        output = nil;
        looper = nil;
    }

    void load (const char* path)
    {
        teardown();

        NSURL* url = [NSURL fileURLWithPath: [NSString stringWithUTF8String: path]];

        NSDictionary* settings = @{
            (NSString*)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA)
        };
        output = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes: settings];

        AVPlayerItem* item = [AVPlayerItem playerItemWithURL: url];
        [item addOutput: output];

        player = [AVPlayer playerWithPlayerItem: item];
        player.volume = 0.0f;
        [player seekToTime: CMTimeMakeWithSeconds(loopStartSec, 600) toleranceBefore:
            kCMTimeZero toleranceAfter: kCMTimeZero];
        [player play];

        looper = [[VBLooper alloc] init];
        looper.player       = player;
        looper.loopStartSec = loopStartSec;
        [[NSNotificationCenter defaultCenter] addObserver: looper
                                                 selector: @selector(itemDidFinish:)
                                                     name: AVPlayerItemDidPlayToEndTimeNotification
                                                   object: item];

        // Periodic time observer to enforce loop-end boundary.
        __block auto* pimpl = this;
        timeObserver = [player addPeriodicTimeObserverForInterval: CMTimeMakeWithSeconds(0.05, 600)
                                                            queue: dispatch_get_main_queue()
                                                       usingBlock: ^(CMTime time) {
            double t = CMTimeGetSeconds (time);
            if (pimpl->loopEndSec > 0 && t >= pimpl->loopEndSec)
            {
                [pimpl->player seekToTime: CMTimeMakeWithSeconds(pimpl->loopStartSec, 600)
                           toleranceBefore: kCMTimeZero
                            toleranceAfter: kCMTimeZero];
            }
        }];
    }

    double getCurrentTime() const
    {
        if (! player) return 0.0;
        CMTime t = [player currentTime];
        if (CMTIME_IS_INVALID(t) || CMTIME_IS_INDEFINITE(t)) return 0.0;
        return CMTimeGetSeconds (t);
    }

    double getDuration() const
    {
        if (! player || ! player.currentItem) return 0.0;
        CMTime d = player.currentItem.duration;
        if (CMTIME_IS_INVALID(d) || CMTIME_IS_INDEFINITE(d)) return 0.0;
        return CMTimeGetSeconds (d);
    }

    void seekTo (double seconds)
    {
        if (! player) return;
        [player seekToTime: CMTimeMakeWithSeconds(seconds, 600)
           toleranceBefore: kCMTimeZero
            toleranceAfter: kCMTimeZero];
    }

    void setLoop (double startSec, double endSec)
    {
        loopStartSec = startSec;
        loopEndSec   = endSec;
        if (looper)
            looper.loopStartSec = startSec;
    }

    bool getLatestFrame (int& outW, int& outH, int& outStride, const uint8_t*& outPixels)
    {
        if (! output || ! player)
            return false;

        CMTime t = [player currentTime];
        if ([output hasNewPixelBufferForItemTime: t])
        {
            CVPixelBufferRef pb = [output copyPixelBufferForItemTime: t itemTimeForDisplay: nullptr];
            if (pb)
            {
                CVPixelBufferLockBaseAddress (pb, kCVPixelBufferLock_ReadOnly);

                const int w      = (int)CVPixelBufferGetWidth (pb);
                const int h      = (int)CVPixelBufferGetHeight (pb);
                const int stride = (int)CVPixelBufferGetBytesPerRow (pb);
                const uint8_t* src = (const uint8_t*)CVPixelBufferGetBaseAddress (pb);

                {
                    std::lock_guard<std::mutex> lock (mtx);
                    pixels.resize ((size_t)h * stride);
                    std::memcpy (pixels.data(), src, pixels.size());
                    frameW      = w;
                    frameH      = h;
                    frameStride = stride;
                }

                CVPixelBufferUnlockBaseAddress (pb, kCVPixelBufferLock_ReadOnly);
                CVPixelBufferRelease (pb);
            }
        }

        std::lock_guard<std::mutex> lock (mtx);
        if (pixels.empty()) return false;
        outW      = frameW;
        outH      = frameH;
        outStride = frameStride;
        outPixels = pixels.data();
        return true;
    }
};

// ── VideoBackground ───────────────────────────────────────────────────────────
VideoBackground::VideoBackground()  : pimpl (std::make_unique<Pimpl>()) {}
VideoBackground::~VideoBackground() = default;

void VideoBackground::loadFile (const char* path) { pimpl->load (path); }
void VideoBackground::play()                      { pimpl->play(); }
void VideoBackground::pause()                     { pimpl->pause(); }
void VideoBackground::stop()                      { pimpl->stop(); }

double VideoBackground::currentTime() const           { return pimpl->getCurrentTime(); }
double VideoBackground::duration() const              { return pimpl->getDuration(); }
void   VideoBackground::seek (double seconds)         { pimpl->seekTo (seconds); }
void   VideoBackground::setLoopPoints (double startSec, double endSec) { pimpl->setLoop (startSec, endSec); }

double VideoBackground::fileDuration (const char* absolutePath)
{
    @autoreleasepool
    {
        NSURL* url = [NSURL fileURLWithPath: [NSString stringWithUTF8String: absolutePath]];
        AVAsset* asset = [AVAsset assetWithURL: url];
        CMTime d = asset.duration;
        if (CMTIME_IS_INVALID(d) || CMTIME_IS_INDEFINITE(d)) return 0.0;
        return CMTimeGetSeconds (d);
    }
}

bool VideoBackground::getLatestFrame (int& outW, int& outH, int& outStride, const uint8_t*& outPixels)
{
    return pimpl->getLatestFrame (outW, outH, outStride, outPixels);
}
