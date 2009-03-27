/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include <Bitmap.h>
#include <Entry.h>
#include <Mime.h>
#include <Resources.h>
#include <Screen.h>
#include <StringView.h>

#ifdef DEBUG
#include <ctime> 
#include <cstdlib> 
#include <iostream>
#include <string.h>
#endif

#include <ScreenSaver.h>

#define APP_SIGNATURE "application/x-vnd.pecora-sheepsaver"

// Class declaration --------------------------
class SheepSaver : public BScreenSaver
{
public:
				SheepSaver(BMessage *message, image_id id);
				~SheepSaver();
	void		StartConfig(BView *view);
	status_t	StartSaver(BView *v, bool preview);
	void		Draw(BView *v, int32 frame);
	bool		FindLineAt( BPoint pt );
	rgb_color	ColorAt( BPoint pt );
	
private:
	
	bool		fInitOk;
	BBitmap		*fImg[5];

};

// MAIN INSTANTIATION FUNCTION
extern "C" _EXPORT BScreenSaver *instantiate_screen_saver(BMessage *message, image_id image)
{
	return new SheepSaver(message, image);
}

// Class definition ---------------------------
SheepSaver::SheepSaver(BMessage *message, image_id image)
 :	BScreenSaver(message, image)
{

	fInitOk = false;
	
	srand(time(NULL));

	// Images holen
	for (int i = 0; i<5; ++i) fImg[i] = 0;

	BMimeType	mime( APP_SIGNATURE );
	entry_ref	app_ref;
	
	BFile file;
	file.SetTo("/boot/home/config/add-ons/Screen Savers/SheepSaver", B_READ_WRITE);

	if (file.InitCheck()!=B_OK) {
#ifdef DEBUG
		cerr << "Cannot find resource file: " << strerror(file.InitCheck()) << endl;
#endif
		return;

	}
	else {

		BResources	resources( &file );
	
		status_t err;
		if ( (err = resources.SetTo(&file)) != B_OK) {
#ifdef DEBUG
			cerr << "Unable to open resource file." << endl;
#endif
			return;
		}
		else {
	
			size_t		groesse;
			BMessage	msg;
			char		*buf;
	
			for (int i = 0; i<5; ++i) {
				buf = (char *)resources.LoadResource('BBMP', i+1, &groesse);
				if (!buf) {
#ifdef DEBUG
					cerr << "Resource not found: " << i+1 << endl;
#endif
				}
				else {
					msg.Unflatten(buf);
					fImg[i] = new BBitmap( &msg );
				}
			}
		}
	}
	
	fInitOk = true;
}

SheepSaver::~SheepSaver() {
	for (int i=0; i<5; ++i) delete fImg[i];
}

void SheepSaver::StartConfig(BView *view)
{
	BStringView *child = new BStringView(BRect(10, 10, 200, 25), B_EMPTY_STRING, "SheepSaver");
	child->SetFont(be_bold_font);
	view->AddChild(child);

	view->AddChild(new BStringView(BRect(8, 30, 200, 55), B_EMPTY_STRING, " ©2001/2002 by Werner Freytag"));
}

status_t SheepSaver::StartSaver(BView *view, bool preview)
{	
	if (!fInitOk) return B_ERROR;
	
	SetTickSize(500);
	return B_OK;
}

void SheepSaver::Draw(BView *view, int32 frame)
{
	if (frame==0) {
		view->SetDrawingMode( B_OP_ALPHA );
	}
	
	BPoint pt( rand() % view->Bounds().IntegerWidth() - 3, rand() % view->Bounds().IntegerHeight() - 3 );

	BBitmap	*bmp;
	if ((float)rand()/RAND_MAX > 0.95 ) {
		bmp = fImg[3 + (int)((float)rand()/RAND_MAX > 0.95 )];
	}
	else {
		bmp = fImg[(int)((float)rand()/RAND_MAX * 3)];
	}
	view->DrawBitmap( bmp, pt );
}
