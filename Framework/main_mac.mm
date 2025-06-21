#include "ggl.h"
#include "scene.h"

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl3.h>
#import <mach/mach_time.h>

// 高精度帧时间计算 (Mac实现)
float GetFrameTime() {
    static uint64_t lastTime = 0;
    static mach_timebase_info_data_t timebase;
    
    if (timebase.denom == 0) {
        mach_timebase_info(&timebase);
    }
    
    uint64_t currentTime = mach_absolute_time();
    uint64_t elapsedNano = (currentTime - lastTime) * timebase.numer / timebase.denom;
    
    lastTime = currentTime;
    return static_cast<float>(elapsedNano) / 1e9f;
}

// macOS OpenGL视图类
@interface MacGLView : NSOpenGLView {
    CVDisplayLinkRef displayLink;
    NSTrackingArea* trackingArea;
}

- (void)initGLContext;
- (void)drawView;
@end

@implementation MacGLView

- (void)prepareOpenGL {
    [super prepareOpenGL];
    [self initGLContext];
    
    // 配置垂直同步
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    // 设置显示链接
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &displayLinkCallback, (__bridge void*)self);
    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(displayLink);
}

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, 
                                   const CVTimeStamp* now, 
                                   const CVTimeStamp* outputTime, 
                                   CVOptionFlags flagsIn, 
                                   CVOptionFlags* flagsOut, 
                                   void* displayLinkContext) {
    @autoreleasepool {
        MacGLView* view = (__bridge MacGLView*)displayLinkContext;
        
        // 确保上下文正确绑定
        [[view openGLContext] makeCurrentContext];
        
        // 渲染帧
        Draw();
        
        // 交换缓冲区
        [[view openGLContext] flushBuffer];
    }
    return kCVReturnSuccess;
}

- (void)drawView {
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    // 调用渲染函数
    Draw();
    
    [[self openGLContext] flushBuffer];
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
}

- (void)initGLContext {
    [[self openGLContext] makeCurrentContext];
    
    // macOS OpenGL初始化
    gladLoadGL();
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    
    // 调用原始初始化代码
    Init();
}

- (void)reshape {
    [super reshape];
    NSRect backRect = [self convertRectToBacking:[self bounds]];
    SetViewPortSize(NSWidth(backRect), NSHeight(backRect));
}

- (void)dealloc {
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    [trackingArea release];
    [super dealloc];
}

// 创建像素格式
+ (NSOpenGLPixelFormat*)createPixelFormat {
    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFADoubleBuffer,                  // 双缓冲
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core, // 核心配置文件
        NSOpenGLPFAColorSize, 24,                 // 8位RGB通道
        NSOpenGLPFAAlphaSize, 8,                  // Alpha通道
        NSOpenGLPFADepthSize, 24,                 // 24位深度缓冲
        NSOpenGLPFAStencilSize, 8,                // 8位模板缓冲
        NSOpenGLPFAAccelerated,                   // 硬件加速
        NSOpenGLPFAMultisample,                   // 多重采样
        NSOpenGLPFASampleBuffers, 1,               // MSAA缓冲
        NSOpenGLPFASamples, 4,                    // 4x MSAA
        NSOpenGLPFANoRecovery,                    // 无回退
        0
    };
    
    return [[[NSOpenGLPixelFormat alloc] initWithAttributes:attrs] autorelease];
}

@end

// 应用入口
int main(int argc, const char * argv[]) {
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    
    // 创建应用
    [NSApplication sharedApplication];
    
    // 创建窗口
    NSRect mainDisplayRect = [[NSScreen mainScreen] frame];
    NSRect windowRect = NSMakeRect(NSMidX(mainDisplayRect) - 640, 
                                   NSMidY(mainDisplayRect) - 360, 
                                   1280, 720);
    
    NSWindow* window = [[NSWindow alloc] initWithContentRect:windowRect
                                                  styleMask:NSWindowStyleMaskTitled | 
                                                            NSWindowStyleMaskClosable |
                                                            NSWindowStyleMaskMiniaturizable |
                                                            NSWindowStyleMaskResizable
                                                    backing:NSBackingStoreBuffered
                                                      defer:NO];
    
    [window setTitle:@"OpenGL macOS Window"];
    
    // 创建OpenGL视图
    NSOpenGLPixelFormat* pixelFormat = [MacGLView createPixelFormat];
    MacGLView* glView = [[MacGLView alloc] initWithFrame:windowRect 
                                           pixelFormat:pixelFormat];
    
    [window setContentView:glView];
    [window center];
    [window makeKeyAndOrderFront:nil];
    [window setDelegate:(id<NSWindowDelegate>)glView];
    
    // 运行主循环
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp run];
    
    [glView release];
    [window release];
    [pool release];
    
    return 0;
}
