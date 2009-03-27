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
#include <Screen.h>
#include <StringView.h>

#include <ctime> 
#include <cstdlib> 
#include <iostream>

#include <ScreenSaver.h>

#define APP_SIGNATURE "application/x-vnd.pecora-sheepssaver"

BBitmap	*FetchResourceBitmap(const char *name);

// Class declaration --------------------------
class SheepsSaver : public BScreenSaver
{
public:
				SheepsSaver(BMessage *message, image_id id);
				~SheepsSaver();
	status_t	InitCheck();
	void		StartConfig(BView *view);
	status_t	StartSaver(BView *v, bool preview);
	void		Draw(BView *v, int32 frame);
	bool		FindLineAt( BPoint pt );

	// data
	BBitmap		*fBitmap;

private:

	BBitmap		*sheep1, *sheep2;

};

// MAIN INSTANTIATION FUNCTION
extern "C" _EXPORT BScreenSaver *instantiate_screen_saver(BMessage *message, image_id image)
{
	return new SheepsSaver(message, image);
}

// Class definition ---------------------------
SheepsSaver::SheepsSaver(BMessage *message, image_id image)
 :	BScreenSaver(message, image),
 	fBitmap( 0 )
{
	srand(time(NULL));
	sheep1 = FetchResourceBitmap("schaf1");
	sheep2 = FetchResourceBitmap("schaf2");
}

SheepsSaver::~SheepsSaver() {
	delete fBitmap;
}


status_t SheepsSaver::InitCheck() {
	return ( sheep1 && sheep2 ) ? B_OK : B_NO_INIT ;
}

void SheepsSaver::StartConfig(BView *view)
{
	view->AddChild(new BStringView(BRect(10, 10, 200, 35), B_EMPTY_STRING, "Sheeps"));
}

status_t SheepsSaver::StartSaver(BView *view, bool preview)
{	
	return B_OK;
}

void SheepsSaver::Draw(BView *view, int32 frame)
{
	if (frame==0) {
		BScreen().GetBitmap(&fBitmap, false);
		view->SetDrawingMode( B_OP_ALPHA );
	}
	else {
//		view->DrawBitmap(fBitmap);
	}
	
	BPoint pt;
	BBitmap	*bmp;

//	do {
		pt.x = (rand() % view->Bounds().IntegerWidth()) - 10;
		pt.y = (rand() % view->Bounds().IntegerHeight()) - 8;
//	} while ( !FindLineAt( pt ) );
	if ( !FindLineAt( BPoint( pt.x, pt.y - 1 ) ) && FindLineAt( pt ) && !FindLineAt( BPoint( pt.x, pt.y + 1 ) )) {
//	 view->SetHighColor( 100, 255, 50 ); else view->SetHighColor( 255, 0, 0 );
	
		view->SetHighColor( 100, 255, 50, 100 );
	}	
else 		view->SetHighColor( 255, 0, 0, 100 );
	view->FillEllipse( pt, 5, 5);


	bmp = ( (float)rand()/RAND_MAX <= 0.95 ) ? sheep1 : sheep2;
//	view->DrawBitmap( bmp, pt );
}

bool SheepsSaver::FindLineAt( BPoint pt ) {

	if (pt.x<5 || pt.y<0 || pt.x>fBitmap->Bounds().Width()-5 || pt.y>fBitmap->Bounds().Height() ) return false;
	
	int		bytesPerColor = (int)(fBitmap->BytesPerRow() / (fBitmap->Bounds().Width() + 1));
	uchar	*bytes = (uchar *)fBitmap->Bits();
	
	bool	foundit = true;
	uint64	startposition = ((uint64)pt.x - 4 ) * bytesPerColor + (uint64)pt.y * fBitmap->BytesPerRow();
	
	// Vergleichsvariablen setzen
	uchar	compare_this[4]; // Achtung: Es könnnen nur Screens bis RGB32 getestet werden!
	for (int i = 0; i<bytesPerColor; ++i ) {
		compare_this[i] = bytes[startposition - bytesPerColor + i];
	}
	
	for (int j = 0; j <9; ++j) {
		for (int i = 0; i<bytesPerColor; ++i ) {
			if (compare_this[i] != bytes[startposition + j * bytesPerColor + i]) {
				foundit = false; break;
			}
		}
	}
	
	return foundit;
}


//-----------------------------------

#include <Resources.h>

BBitmap	*FetchResourceBitmap(const char *name) {

	BMimeType	mime( APP_SIGNATURE );
	entry_ref	app_ref;
	
	if ( mime.GetAppHint(&app_ref) != B_OK ) return 0;
	
	BFile file(&app_ref, B_READ_WRITE);

	BResources	resources( &file );

	status_t err;
	if ((err = resources.SetTo(&file)) != B_OK) {
		cerr << "Unable to open resource file" << endl;
		return 0;
	}

	size_t		groesse;
	BMessage	msg;
	char		*buf;

	buf = (char *)resources.LoadResource('BBMP', name, &groesse);
	if (!buf) {
		cerr << "Resource not found: " << name << endl;
		return 0;
	}
	msg.Unflatten(buf);
	return new BBitmap( &msg );
};
