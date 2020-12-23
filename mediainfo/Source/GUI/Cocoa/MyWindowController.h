/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//  Created by Max Pozdeev on 07.02.12.

#import <Cocoa/Cocoa.h>
#import "oMediaInfoList.h"
#import "HyperlinkButton.h"

typedef enum { Kind_Text, Kind_XML, Kind_JSON, Kind_PBCore, Kind_PBCore2, Kind_reVTMD, Kind_MPEG7, Kind_EBUCore_1_5, Kind_EBUCore_1_6, Kind_EBUCore_1_8_ps, Kind_EBUCore_1_8_sp, Kind_EBUCore_1_8_ps_json, Kind_EBUCore_1_8_sp_json, Kind_FIMS_1_1, Kind_FIMS_1_2, Kind_FIMS_1_3, Kind_NISO_Z39_87} ViewMenu_Kind;

@interface MyWindowController : NSWindowController {

    IBOutlet NSArrayController *comboController;
    IBOutlet NSTabView *tabs;
    IBOutlet NSSegmentedControl *tabSelector;
    IBOutlet NSOutlineView *treeOutline;
    IBOutlet NSTreeController *treeOutlineController;
    IBOutlet NSTextView *textField;
    IBOutlet NSMenu *otherViewsMenu;
    IBOutlet NSView *formatSelectionAccView;
    IBOutlet NSPopUpButton *exportFormatButton;
    IBOutlet NSTextField *easyGeneralTextLeft;
    IBOutlet NSTextField *easyGeneralTextRight;
    IBOutlet HyperlinkButton *easyGeneralLinkButton;
    IBOutlet NSArrayController *easyStreamsController;
    IBOutlet NSTableView *easyTable;

    oMediaInfoList *mediaList;
    NSInteger selectedFileIndex;

    ViewMenu_Kind _lastTextKind;
    NSSavePanel *_exportSavePanel;
}

//@property (assign) NSInteger selectedFileIndex;

-(IBAction)openFile:(id)sender;
-(IBAction)clickOnViewSelector:(id)sender;
-(IBAction)selectEasyTab:(id)sender;
-(IBAction)selectTreeTab:(id)sender;
-(IBAction)selectTextTab:(id)sender;

-(void)_selectViewOFKind:(ViewMenu_Kind)_kind;
-(IBAction)selectViewXML:(id)sender;
-(IBAction)selectViewJSON:(id)sender;
-(IBAction)selectViewMPEG7:(id)sender;
-(IBAction)selectViewPBCore:(id)sender;
-(IBAction)selectViewPBCore2:(id)sender;
-(IBAction)selectViewEBUCore15:(id)sender;
-(IBAction)selectViewEBUCore16:(id)sender;
-(IBAction)selectViewEBUCore18_ps:(id)sender;
-(IBAction)selectViewEBUCore18_sp:(id)sender;
-(IBAction)selectViewEBUCore18_ps_json:(id)sender;
-(IBAction)selectViewEBUCore18_sp_json:(id)sender;
-(IBAction)selectViewFIMS11:(id)sender;
-(IBAction)selectViewFIMS12:(id)sender;
-(IBAction)selectViewFIMS13:(id)sender;
-(IBAction)selectViewReVTMD:(id)sender;
-(IBAction)selectViewNISO_Z39_87:(id)sender;

-(IBAction)export:(id)sender;
-(IBAction)changeExportFormat:(id)sender;

-(NSInteger)selectedFileIndex;
-(void)setSelectedFileIndex:(NSInteger)index;

-(void)processFiles:(NSArray *)URLs;
-(void)showFileAtIndex:(NSUInteger)index;
-(void)updateEasyTabWithFileAtIndex:(NSUInteger)index;
-(void)updateTextTabWithFileAtIndex:(NSUInteger)index;
-(void)_updateTextTabWithContentOfAttributedTextAtIndex:(NSUInteger)index;
-(void)_updateTextTabWithContentOfSimpleTextAtIndex:(NSUInteger)index;

@end
