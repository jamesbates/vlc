/*****************************************************************************
 * MainWindow.h: MacOS X interface module
 *****************************************************************************
 * Copyright (C) 2002-2012 VLC authors and VideoLAN
 * $Id$
 *
 * Authors: Felix Paul Kühne <fkuehne -at- videolan -dot- org>
 *          Jon Lech Johansen <jon-vl@nanocrew.net>
 *          Christophe Massiot <massiot@via.ecp.fr>
 *          Derk-Jan Hartman <hartman at videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#import <Cocoa/Cocoa.h>
#import "CompatibilityFixes.h"
#import "PXSourceList.h"
#import "PXSourceListDataSource.h"

#import <vlc_input.h>
#import <vlc_vout_window.h>

#import "Windows.h"
#import "misc.h"
#import "fspanel.h"
#import "MainWindowTitle.h"

@class VLCDetachedVideoWindow;
@class VLCMainWindowControlsBar;
@class VLCVoutView;

@interface VLCMainWindow : VLCVideoWindowCommon <PXSourceListDataSource, PXSourceListDelegate, NSWindowDelegate, NSAnimationDelegate, NSSplitViewDelegate> {

    IBOutlet id o_search_fld;
    
    IBOutlet id o_playlist_table;
    IBOutlet id o_split_view;
    IBOutlet id o_left_split_view;
    IBOutlet id o_right_split_view;
    IBOutlet id o_sidebar_view;
    IBOutlet id o_sidebar_scrollview;
    IBOutlet id o_chosen_category_lbl;

    IBOutlet id o_dropzone_view;
    IBOutlet id o_dropzone_btn;
    IBOutlet id o_dropzone_lbl;
    IBOutlet id o_dropzone_box;

    IBOutlet VLCFSPanel *o_fspanel;

    IBOutlet id o_podcast_view;
    IBOutlet id o_podcast_add_btn;
    IBOutlet id o_podcast_remove_btn;
    IBOutlet id o_podcast_subscribe_window;
    IBOutlet id o_podcast_subscribe_title_lbl;
    IBOutlet id o_podcast_subscribe_subtitle_lbl;
    IBOutlet id o_podcast_subscribe_url_fld;
    IBOutlet id o_podcast_subscribe_cancel_btn;
    IBOutlet id o_podcast_subscribe_ok_btn;
    IBOutlet id o_podcast_unsubscribe_window;
    IBOutlet id o_podcast_unsubscribe_title_lbl;
    IBOutlet id o_podcast_unsubscribe_subtitle_lbl;
    IBOutlet id o_podcast_unsubscribe_pop;
    IBOutlet id o_podcast_unsubscribe_ok_btn;
    IBOutlet id o_podcast_unsubscribe_cancel_btn;

    BOOL b_nativeFullscreenMode;
    BOOL b_video_playback_enabled;
    BOOL b_dropzone_active;
    BOOL b_splitview_removed;
    BOOL b_minimized_view;

    int i_lastSplitViewHeight;

    NSMutableArray *o_sidebaritems;

    BOOL              b_nonembedded;
    BOOL              b_podcastView_displayed;

    VLCWindow       * o_fullscreen_window;
    NSViewAnimation * o_fullscreen_anim1;
    NSViewAnimation * o_fullscreen_anim2;
    NSViewAnimation * o_makekey_anim;
    NSView          * o_temp_view;
    /* set to yes if we are fullscreen and all animations are over */
    BOOL              b_fullscreen;
    BOOL              b_window_is_invisible;
    NSRecursiveLock * o_animation_lock;

    NSTimer *t_hide_mouse_timer;

    VLCColorView * o_color_backdrop;
    NSInteger i_originalLevel;

    id o_current_video_window;

    NSRect frameBeforePlayback;
}
+ (VLCMainWindow *)sharedInstance;
@property (readonly) BOOL fullscreen;

- (VLCMainWindowControlsBar *)controlsBar;

- (IBAction)togglePlaylist:(id)sender;

- (IBAction)dropzoneButtonAction:(id)sender;

- (IBAction)addPodcast:(id)sender;
- (IBAction)addPodcastWindowAction:(id)sender;
- (IBAction)removePodcast:(id)sender;
- (IBAction)removePodcastWindowAction:(id)sender;

- (void)windowResizedOrMoved:(NSNotification *)notification;

- (void)showDropZone;
- (void)hideDropZone;
- (void)showSplitView;
- (void)hideSplitView;
- (void)updateTimeSlider;
- (void)updateWindow;
- (void)updateName;
- (void)setPause;
- (void)setPlay;
- (void)updateVolumeSlider;

- (VLCVoutView *)setupVout:(vout_window_t *)p_wnd;
- (void)setVideoplayEnabled;

- (void)hideMouseCursor:(NSTimer *)timer;
- (void)recreateHideMouseTimer;

/* fullscreen handling */
- (void)showFullscreenController;
- (void)lockFullscreenAnimation;
- (void)unlockFullscreenAnimation;
- (void)enterFullscreen;
- (void)leaveFullscreen;
- (void)leaveFullscreenAndFadeOut: (BOOL)fadeout;
- (void)hasEndedFullscreen;
- (void)hasBecomeFullscreen;

/* lion's native fullscreen handling */
- (void)windowWillEnterFullScreen:(NSNotification *)notification;
- (void)windowDidEnterFullScreen:(NSNotification *)notification;
- (void)windowWillExitFullScreen:(NSNotification *)notification;

@end

@interface VLCDetachedVideoWindow : VLCVideoWindowCommon

@end