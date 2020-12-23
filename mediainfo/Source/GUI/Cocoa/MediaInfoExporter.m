/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//  Created by Max Pozdeev on 14.02.12.

#import "MediaInfoExporter.h"


@implementation MediaInfoExporter

@synthesize extensionHidden;

- (id)initWithObj:(oMediaInfoList*)obj {
	if(self = [super init]) {
		mil = [obj retain];
		extensionHidden = NO;
	}
	return self;
}

- (void)dealloc {
	[mil release];
	mil = nil;
	[super dealloc];
}


#pragma mark -

- (BOOL)exportToText:(NSURL*)url
{
	NSString *text = [mil inform];
	
	return [self saveData:[text dataUsingEncoding:NSUTF8StringEncoding] toUrl:url];
	
}

- (BOOL)exportFormat:(NSString*)format toUrl:(NSURL*)url
{
	[mil setOption:@"Inform" withValue:format];
	
	NSString *text = [mil inform];
	
	[mil setOption:@"Inform" withValue:@""]; //reset
	
	return [self saveData:[text dataUsingEncoding:NSUTF8StringEncoding] toUrl:url];
}


- (BOOL)saveData:(NSData*)aData toUrl:(NSURL*)url {
	
	NSFileManager *fmgr = [[NSFileManager alloc] init]; //thread-safe instance
	NSString *path = [url path];
	BOOL fileExists = [fmgr fileExistsAtPath:path];
	
	if(![aData writeToURL:url atomically:NO]) {
		[fmgr release];
		return NO;
	}
	
	//set attributes only for newly created file
	if(!fileExists && extensionHidden) {
		
		NSDictionary *attrs = [NSDictionary dictionaryWithObjectsAndKeys:
							   [NSNumber numberWithBool:extensionHidden], NSFileExtensionHidden,
							   nil];
		
		[fmgr setAttributes:attrs ofItemAtPath:path error:nil];
	}
	[fmgr release];
	
	return YES;
}

@end
