#import <Foundation/Foundation.h>
#import <Foundation/NSUserNotification.h>
#import <objc/runtime.h>
#import "ssh-agent-notify.h"

@implementation NSBundle(sshagent)
- (NSString *)__bundleIdentifier
{
	return (self == [NSBundle mainBundle] ? @"com.apple.keychainaccess" :
	    [self __bundleIdentifier]);
}
@end

BOOL
installNSBundleHook()
{
	Class class = objc_getClass("NSBundle");
	if (class) {
		method_exchangeImplementations(
		  class_getInstanceMethod(class, @selector(bundleIdentifier)),
		  class_getInstanceMethod(class, @selector(__bundleIdentifier))
		);
		return YES;
	}
	return NO;
}

#pragma mark - NotificationCenterDelegate

@interface NotificationCenterDelegate:NSObject<NSUserNotificationCenterDelegate>
@property (nonatomic, assign) BOOL keepRunning;
@end

@implementation NotificationCenterDelegate
- (void)userNotificationCenter:(NSUserNotificationCenter *)center
    didDeliverNotification:(NSUserNotification *)notification
{
	self.keepRunning = NO;
}
@end

#pragma mark -

void
notify_user_macos(char *key, char *comment)
{
	@autoreleasepool
	{
		if (!installNSBundleHook()) return;

		NSUserNotificationCenter *center =
		    [NSUserNotificationCenter defaultUserNotificationCenter];
		NotificationCenterDelegate *ncDelegate =
		    [[NotificationCenterDelegate alloc] init];
		ncDelegate.keepRunning = YES;
		center.delegate = ncDelegate;

		NSUserNotification *notification =
		    [[NSUserNotification alloc] init];
		[notification setTitle:
		    @"Key challenge signed for fingerprint"];
		[notification setSubtitle:
		    [NSString stringWithUTF8String:key]];
		[notification setInformativeText:
		    [NSString stringWithUTF8String:comment]];
		[notification setSoundName:@"Submarine"];
		[center scheduleNotification:notification];
	}
}
