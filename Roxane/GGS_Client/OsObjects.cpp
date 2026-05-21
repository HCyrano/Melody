/*
 * Copyright (C) 2001  Chris Welty
 * Copyright (C) 2026  bruno Causse (modified)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <sstream>
#include <iomanip>
#include <ctime>
#include <cctype>

#include "OsObjects.hpp"


void COsMove::In(std::istream& is) {
	char cCol;

	is >> cCol;
	cCol=toupper(cCol);
	fPass= cCol=='P';
	if (fPass) {
		while (isalpha(is.peek()) && is>>cCol) {};
	}
	else {
		col=cCol-'A';
		is >> row;
		row--;
	}
}

void COsMove::Out(std::ostream& os) const {
	if (fPass)
		os << "PA";
	else
		os << (char)(col+'A') << row+1;
}

void COsMove::SetIOS(int iosmove) {
	fPass=false;
	if (iosmove<0)
		iosmove=-iosmove;
	row=(iosmove%10)-1;
	col=(iosmove/10)-1;
}

bool COsMove::operator==(const COsMove& b) const {
	if (fPass || b.fPass)
		return fPass && b.fPass;
	else
		return row==b.row && col==b.col;
}

bool COsMove::operator<(const COsMove& b) const {
	if (fPass!=b.fPass)
		return fPass;
	else if (row!=b.row)
		return row<b.row;
	else
		return col<b.col;
}

void IgnoreAlnum(std::istream& is) {
	while (isalnum(is.peek()))
		is.ignore(1);
}

//Debug 1
//parse string GGS move E2/16.00/109.00 to COsMovelistItem
void COsMoveListItem::In(std::istream& is) {

	// move
	is >> mv;
	IgnoreAlnum(is);

	// eval
	dEval=0;
	if (is.peek()=='/') {
		is.ignore(1);
		is >> dEval;

		// 0 is passed as a blank spot, so ignore errors here
		if (is.fail()&&!is.bad())
			is.clear();
	}

	tElapsed=0;
	if (is.peek()=='/') {
		is.ignore(1);
		is >> tElapsed;
	}
}

void COsMoveListItem::Out(std::ostream& os) const {
	long nPrecisionOld=os.precision(2);
	std::ios_base::fmtflags fFlagsOld=os.setf(std::ios::fixed, std::ios::floatfield);

	os << mv;
	if (dEval !=0 || tElapsed != 0) {
		os << '/';
		if (dEval != 0)
			os << dEval;
		if (tElapsed != 0)
			os << '/' << tElapsed;
	}

	os.setf(fFlagsOld);
	os.precision(nPrecisionOld);
}

void COsMoveList::Update(const COsMoveListItem& mli) {
	push_back(mli);
}

/////////////////////////////////////////
// COsClock
/////////////////////////////////////////

COsClock::COsClock() {
}

COsClock::COsClock(double atCurrent, double atIncrement, double atGrace, int aiTimeout) {
	tCurrent=atCurrent;
	tIncrement=atIncrement;
	tGrace=atGrace;
	iTimeout=aiTimeout;
}

void COsClock::In(std::istream& is) {
	Clear();

	tCurrent=ReadTime(is);

	if (is.peek()=='/') {
		is.ignore(1);
		// 0 is passed as a blank spot
		if (is.peek()=='/')
			tIncrement=0;
		else 
			tIncrement=ReadTime(is);
	}

	if (is.peek()=='/') {
		is.ignore(1);
		tGrace=ReadTime(is);
	}
}

std::istream& COsClock::InIOS(std::istream& is) {
	char c;

	is >> c;
    if (c != '(') { std::cerr << "[GGS] Warning: COsClock::InIOS malformed, expected '('" << std::endl; return is; }
    is >> tCurrent;
	tCurrent*=60;
	is >> tIncrement;
	is >> tGrace;
	tGrace*=60;
	is >> c;
    if (c != ')') { std::cerr << "[GGS] Warning: COsClock::InIOS malformed, expected ')'" << std::endl; return is; }

	return is;
}

void COsClock::Out(std::ostream& os) const {
	WriteTime(os, static_cast<int>(tCurrent));
	if (tIncrement!=0  || tGrace!=0) {
		os << "/";
		if (tIncrement!=0)
			WriteTime(os, static_cast<int>(tIncrement));
		if (tGrace!=0) {
			os << "/";
			WriteTime(os, static_cast<int>(tGrace));
		}
	}
}

void COsClock::Clear() {
	iTimeout=0;
	tCurrent=tGrace=tIncrement=0;
}

void COsClock::Update(double tElapsed, bool fIncludeIncrement) {
    
	tCurrent-=tElapsed;

	// adjust for timeouts and grace periods
	if (tCurrent<0) {
		if (iTimeout==0) {
			tCurrent+=tGrace;
			iTimeout=1;
		}
		if (tCurrent<0) {
			tCurrent=0;
			iTimeout=2;
		}
	}
	if (fIncludeIncrement && iTimeout<2)
		tCurrent+=tIncrement;
}

double COsClock::ReadTime(std::istream& is) {
	int n[4], i;
	char c;
	double t;

	// parse into numbers
	// format is days.hours:minutes:seconds
	// leading 0s can be skipped
	for (i=0; (i<4) && (is>>n[i]); ) {
		i++;
		c=is.peek();
        if (c == '.' && i != 0) {
            std::cerr << "[GGS] Warning: COsClock::ReadTime unexpected '.' at position " << i << std::endl;
            break;
        }
        if (c=='.' || c==':')
			is.ignore(1);
		else
			break;
	}

	// This can fail because the stream is bad, in which case we just return 0
	if (!i)
		return 0;

	// turn it into seconds
	i--;
	t=n[i];
	if (i--) {
		t+=n[i]*60;
		if (i--) {
			t+=n[i]*60*60;
			if (i--) {
				t+=n[i]*24*60*60;
			}
		}
	}

	return t;
}

void COsClock::WriteTime(std::ostream& os, int nSeconds) {
	os << std::setfill('0');

	if (nSeconds>60) {
		int nMinutes=nSeconds/60;
		nSeconds%=60;
		if (nMinutes>60) {
			int nHours=nMinutes/60;
			nMinutes%=60;
			os << nHours << ":" << std::setw(2) << nMinutes;
		}
		else
			os << nMinutes << ":";
		os << std::setw(2) << nSeconds;
	}
	else
		os << nSeconds;

	os <<  std::setfill(' ');
}

bool COsClock::operator==(const COsClock& b) const {
	return iTimeout==b.iTimeout &&
		tCurrent==b.tCurrent &&
		tGrace==b.tGrace &&
		tIncrement==b.tIncrement;
}

bool COsClock::operator!=(const COsClock& b) const {
	return !(*this==b);
}

bool COsClock::operator<=(const COsClock& b) const {
	return iTimeout<=b.iTimeout &&
		tCurrent<=b.tCurrent &&
		tGrace<=b.tGrace &&
		tIncrement<=b.tIncrement;
}

bool COsClock::operator>=(const COsClock& b) const {
	return iTimeout>=b.iTimeout &&
		tCurrent>=b.tCurrent &&
		tGrace>=b.tGrace &&
		tIncrement>=b.tIncrement;
}

bool COsClock::EqualsToNearestSecond(const COsClock& b) const {
	return iTimeout==b.iTimeout &&
		(int)tCurrent==(int)b.tCurrent &&
		(int)tGrace==(int)b.tGrace &&
		(int)tIncrement==(int)b.tIncrement;
}

void COsBoardType::In(std::istream& is) {
	Clear();

	is >> n;
	switch(n) {
	case 4:
	case 6:
	case 8:
	case 10:
	case 12:
	case 14:
		fOcto=false;
		break;
	case 88:
		n=10;
		fOcto=true;
		break;
	default:
		n=8;
		fOcto=false;
		is.setstate(std::ios::failbit);
		break;
	}
}

void COsBoardType::Out(std::ostream& os) const {
	if (fOcto)
		os << 88;
	else
		os << n;
}

void COsBoardType::Clear() {
	n=0;
	fOcto=false;
}

bool COsBoardType::operator==(const COsBoardType& b) const {
	return n==b.n && fOcto==b.fOcto;
}

int COsBoardType::NRandDiscsMax() const {
	if (fOcto)
		return 44;
	else
		return n*n-16;
}

int COsBoardType::NRandDiscsMin() const {
	return 4;
}

int COsBoardType::NPlayableSquares() const {
	if (fOcto)
		return 88;
	else
		return n*n;
}

int COsBoardType::NTotalSquares() const {
	return (n+2)*(n+2);
}

bool COsBoardType::DummyCorner(int row, int col) const {
	if (fOcto) {
		int rowMirror=n-1-row;
		int colMirror=n-1-col;
		
		if (rowMirror<row)
			row=rowMirror;
		if (colMirror<col)
			col=colMirror;

		return (row+col)<2;
	}
	else
		return false;
}

const char* COsBoardType::Description() const {
	if (fOcto)
		return "Octagon";
	else switch(n) {
		case 4: return "4x4";
		case 6: return "6x6";
		case 8: return "8x8";
		case 10: return "10x10";
		case 12: return "12x12";
		case 14: return "14x14";
		default: return "Unknown board";
	}
}

void COsMatchType::In(std::istream& is) {
	Clear();
	bool fOK=false;	// if we get a board type, clear EOFs if we peek too far

	is >> std::ws;
	while (isalnum(is.peek())) {
		char c;
		c=is.peek();
		if (isdigit(c)) {
			is >> bt;
			fOK=true;
		}
		else{
			is >> c;
			switch(tolower(c)) {
			case 's':
				fSynch=true;
				break;
			case 'a':
				fAnti=true;
				break;
			case 'k':
				fKomi=true;
				break;
			case 'r':
				fRand=true;
				is >> nRandDiscs;
				break;
			case 'b':
				fBlack=true;
				break;
			case 'w':
				fWhite=true;
				break;
			}
		}
	}
    if (fBlack && fWhite) {
        std::cerr << "[GGS] Warning: COsMatchType both black and white color specified" << std::endl;
        fWhite = false;
    }
    if (fOK && is.eof() && !is.bad())
		is.clear();
}

void COsMatchType::Out(std::ostream& os) const {
	if (fSynch)
		os << 's';
	os << bt;
	if (fKomi)
		os << 'k';
	if (fAnti)
		os << 'a';
	if (fRand)
		os << 'r' << nRandDiscs;
}

void COsMatchType::Clear() {
	fSynch=fRand=fKomi=fAnti=fBlack=fWhite=false;
	bt.Clear();
}

		
int COsMatchType::Validate() const {
	// color errors, may not specify a color with anything else
	if (fBlack && fWhite)
		return kErrBlackAndWhite;
	if (fBlack || fWhite) {
		if (fRand)
			return kErrRandAndColor;
		if (fSynch)
			return kErrSynchAndColor;
		if (fKomi)
			return kErrKomiAndColor;
	}

	// misc errors
	if (fSynch && fKomi)
		return kErrSynchAndKomi;

	// rand errors
	if (fRand) {
		if (!fSynch && !fKomi)
			return kErrRandUnbalanced;
		if (nRandDiscs > bt.NRandDiscsMax())
			return kErrTooManyRandDiscs;
		else if (nRandDiscs < bt.NRandDiscsMin())
			return kErrTooFewRandDiscs;
	}

	return 0;
}

void COsRatingType::In(std::istream& is) {
	fRand=fAnti=0;
	is >> std::ws;
	while (isalnum(is.peek())) {
		char c;
		c=is.peek();
		if (isdigit(c))
			is >> bt;
		else{
			is >> c;
			switch(tolower(c)) {
			case 'a':
				fAnti=true;
				break;
			case 'r':
				fRand=true;
				break;
			}
		}
	}
}

void COsRatingType::Out(std::ostream& os) const {
	os << bt;
	if (fAnti)
		os << 'a';
	if (fRand)
		os << 'r';
}

void COsBoard::Initialize(const COsBoardType& abt) {
	int row,col;
	char c;
	bt=abt;

	sBoard.resize(bt.NTotalSquares());
	for (row=-1; row<=bt.n; row++) {
		for (col=-1;col<=bt.n; col++) {
			if (row==-1 || col==-1 || row==bt.n || col==bt.n)
				c=DUMMY;
			else if (bt.DummyCorner(row,col))
				c=DUMMY;
			else
				c=EMPTY;
			SetPiece(row,col,c);
		}
	}

	int center=bt.n/2;
	SetPiece(center, center-1,BLACK);
	SetPiece(center-1, center,BLACK);
	SetPiece(center, center,WHITE);
	SetPiece(center-1, center-1,WHITE);

	fBlackMove=true;
}

char COsBoard::Piece(int row, int col) const {
	// need to adjust for the first row and column of dummy squares
	row++;
	col++;
	return sBoard[row*(bt.n+2)+col];
}

void COsBoard::SetPiece(int row, int col, char piece) {
	// need to adjust for the first row and column of dummy squares
	row++;
	col++;
	sBoard[row*(bt.n+2)+col]=piece;
}

void COsBoard::Update(const COsMove& mv) {
	int dRow, dCol;
	char cMover, cOpponent;

	if (!mv.fPass) {

        if (mv.row>=bt.n || mv.col>=bt.n || mv.row<0 || mv.col<0) {
            std::cerr << "[GGS] Warning: COsBoard::Update move out of bounds row="
                      << mv.row << " col=" << mv.col << std::endl;
            return;
        }
        
		if (Piece(mv.row, mv.col)!=EMPTY) {
			OutFormatted(std::cerr);
		}
		else {

			// get colors
			if (fBlackMove) {
				cMover=BLACK;
				cOpponent=WHITE;
			}
			else {
				cMover=WHITE;
				cOpponent=BLACK;
			}

			// update board
			int nFlipped=0;
			SetPiece(mv.row,mv.col,cMover);
			for (dRow=-1; dRow<=1; dRow++) {
				for (dCol=-1; dCol<=1; dCol++) {
					if (dRow || dCol)
						nFlipped+=UpdateDirection(mv.row, mv.col, dRow, dCol,cMover, cOpponent);
				}
			}
            if (!nFlipped)
                std::cerr << "[GGS] Warning: COsBoard::Update illegal move, nothing flipped" << std::endl;		}
	}

	fBlackMove=!fBlackMove;
}

int COsBoard::NPass() const {
	if (HasLegalMove(fBlackMove))
		return 0;
	else if(HasLegalMove(!fBlackMove))
		return 1;
	else
		return 2;
}

bool COsBoard::GameOver() const {
	return !HasLegalMove(fBlackMove) && !HasLegalMove(!fBlackMove);
}

bool COsBoard::HasLegalMove(bool fBlackMover) const {
	int r, c;

	for (r=0; r<bt.n; r++) {
		for (c=0; c<bt.n; c++)
			if (IsMoveLegal(r,c,fBlackMover))
				return true;
	}
	return false;
}

bool COsBoard::operator==(const COsBoard& b) const {
	return bt==b.bt && sBoard == b.sBoard && fBlackMove == b.fBlackMove;
}

std::vector<COsMove> COsBoard::GetMoves(bool fMover) const {
	int r, c;
	std::vector<COsMove> mvs;

	for (r=0; r<bt.n; r++) {
		for (c=0; c<bt.n; c++)
			if (IsMoveLegal(r,c,fMover?fBlackMove:!fBlackMove))
				mvs.push_back(COsMove(r,c));
	}

	return mvs;
}

int COsBoard::IsMoveLegal(const COsMove& move) const {
	if (move.fPass)
		return !HasLegalMove();
	else
		return IsMoveLegal(move.row, move.col);
}

int COsBoard::IsMoveLegal(int row, int col, bool fBlackMover) const {
	int dRow, dCol;
	char cMover, cOpponent;

	if(Piece(row, col)!=EMPTY)
		return 0;

	// get colors
	if (fBlackMover) {
		cMover=BLACK;
		cOpponent=WHITE;
	}
	else {
		cMover=WHITE;
		cOpponent=BLACK;
	}

	// update board
	int nFlipped=0;
	for (dRow=-1; dRow<=1; dRow++) {
		for (dCol=-1; dCol<=1; dCol++) {
			if (dRow || dCol)
				nFlipped+=IsMoveLegalDirection(row, col, dRow, dCol,cMover, cOpponent);
		}
	}
	return nFlipped;
}

void COsBoard::In(std::istream& is) {
	COsBoardType bt;
	char c;
	int i, nsq;

	Clear();

	is >> bt;
	Initialize(bt);
	nsq=bt.NTotalSquares();
	for (i=0; i<nsq; i++) {
		// find the next non-dummy square
		while (sBoard[i]=='d' && i<nsq)
			i++;
		if (i==nsq)
			break;

		// put something there
		is >> std::ws >> c;
		sBoard[i]=c;
        if (c!=BLACK && c!=WHITE && c!=EMPTY)
            std::cerr << "[GGS] Warning: COsBoard::In unexpected piece='" << c << "'" << std::endl;
    }

    if (!is)
        std::cerr << "[GGS] Warning: COsBoard::In stream error after reading board" << std::endl;

	is >> std::ws >> c;

	/*
	// a previous version neglected to save the color to move;
	//	since this was always black we can correct it with this
	//	hack, which will eventuall be removed
	if (!is)
		fBlackMove=true;
	*/
		
	if(c == WHITE)
		fBlackMove = false;
}

void COsBoard::Out(std::ostream& os) const {
	os << bt << " ";
	int i;
	for (i=0; i<bt.NTotalSquares(); i++) {
		if (sBoard[i]!=DUMMY)
			os << sBoard[i];
	}
	os << " " << CMover();
}

void COsBoard::Clear() {
	bt.Clear();
	fBlackMove=true;
	sBoard.erase();
}

void COsBoard::OutFormatted(std::ostream& os) const {
	OutHeader(os);

	int r,c;
	for (r=0; r<bt.n; r++) {
		os << std::setw(2) << r+1;
		for (c=0; c<bt.n; c++) {
			os << " " << Piece(r,c);
		}
		os << std::setw(2) << r+1 << "\n";
	}

	OutHeader(os);
	os << (fBlackMove?"Black":"White") << " to move\n";
}

void COsBoard::OutHeader(std::ostream& os) const {
	int c;

	os << "  ";
	for (c=0; c<bt.n; c++)
		os << " " << (char)(c+'A');
	os << "  \n";
}

int COsBoard::UpdateDirection(int row, int col, int dRow, int dCol, char cMover, char cOpponent) {
	int rowEnd,colEnd;
	int nFlipped;
	nFlipped=0;

	for (rowEnd=row+dRow, colEnd=col+dCol; Piece(rowEnd,colEnd)==cOpponent; rowEnd+=dRow, colEnd+=dCol);
	if (Piece(rowEnd,colEnd)==cMover) {
		for (rowEnd=row+dRow, colEnd=col+dCol; Piece(rowEnd,colEnd)==cOpponent; rowEnd+=dRow, colEnd+=dCol) {
			SetPiece(rowEnd, colEnd,cMover);
			nFlipped++;
		}
	}

	return nFlipped;
}

int COsBoard::IsMoveLegalDirection(int row, int col, int dRow, int dCol, char cMover, char cOpponent) const {
	int rowEnd,colEnd;
	int nFlipped;
	nFlipped=0;

	for (rowEnd=row+dRow, colEnd=col+dCol; Piece(rowEnd,colEnd)==cOpponent; rowEnd+=dRow, colEnd+=dCol);
	if (Piece(rowEnd,colEnd)==cMover) {
		for (rowEnd=row+dRow, colEnd=col+dCol; Piece(rowEnd,colEnd)==cOpponent; rowEnd+=dRow, colEnd+=dCol) {
			nFlipped++;
		}
	}

	return nFlipped;
}

char* COsBoard::GetText(char* sBoard, bool &afBlackMove, bool fTrailingNull) const {
	int r, c;
	char* p;

	p=sBoard;
	for (r=0; r<=bt.n; r++) {
		for (c=0; c<=bt.n; c++) {
			if (Piece(r,c)!=DUMMY) {
				*p=Piece(r,c);
				p++;
			}
		}
	}
	if (fTrailingNull)
		*p='\0';

	afBlackMove=fBlackMove;

    if (p-sBoard != bt.NPlayableSquares())
        std::cerr << "[GGS] Warning: COsBoard::GetText square count mismatch, got "
                  << (p-sBoard) << " expected " << bt.NPlayableSquares() << std::endl;
    
    return sBoard;
}

std::string COsBoard::fromGGS() const {

	std::string board("[");
	for (int r=0; r<=bt.n; r++) {
			board += " ";
		for (int c=0; c<=bt.n; c++) {
			if (Piece(r,c)!=DUMMY) {
				board +=Piece(r,c);
			}
		}
	}
	
	board += fBlackMove? "*]" : "O]";
	
	return board;
}

void COsBoard::SetText(const char* sBoard) {
	int r, c;
	const char* p;

	p=sBoard;
	for (r=0; r<=bt.n; r++) {
		for (c=0; c<=bt.n; c++) {
			if (Piece(r,c)!=DUMMY) {
				SetPiece(r,c,*p);
				p++;
			}
		}
	}
}

char COsBoard::CMover() const {
	return fBlackMove?BLACK:WHITE;
}

void COsBoard::GetPieceCounts(int& nBlack, int& nWhite, int& nEmpty) const {
	nBlack=nWhite=nEmpty=0;
	int i;
	for (i=0; i<bt.NTotalSquares(); i++) {
		switch(sBoard[i]) {
		case BLACK: nBlack++; break;
		case WHITE: nWhite++; break;
		case EMPTY: nEmpty++; break;
		case DUMMY: break;
		}
	}
}

int COsBoard::NetBlackSquares() const {
	int nBlack, nWhite, nEmpty;
	GetPieceCounts(nBlack, nWhite, nEmpty);
	return nBlack-nWhite;
}

int COsBoard::Result(bool fAnti) const {
	int nBlack, nWhite, nEmpty, nNet;
	GetPieceCounts(nBlack, nWhite, nEmpty);
	nNet=nBlack-nWhite;
	if (nNet<0)
		nNet-=nEmpty;
	else if (nNet>0)
		nNet+=nEmpty;

	return fAnti?nNet:-nNet;
}

/////////////////////////////////////
// COsDateTime
/////////////////////////////////////

void COsDateTime::In(std::istream& is) {
	is >> day >> sMonth >> year >> sTime;
}

std::string COsDateTime::Text() const {
	std::ostringstream os;
	os << std::setw(4) << year << "-" << sMonth << "-" << day << " " << sTime << '\0';
	std::string result(os.str());
	//os.rdbuf()->freeze(0); inutile avec sstream

	return result;
}

/////////////////////////////////////
// COsPosition
/////////////////////////////////////

//debug
void COsPosition::Update(const COsMoveListItem& mli) {
    cks[board.fBlackMove].Update(mli.tElapsed);
	board.Update(mli.mv);
}

void COsPosition::Update(const COsMoveList& ml, unsigned long nMoves) {
	if (nMoves>ml.size())
		nMoves=ml.size();
	for (unsigned long i=0; i<nMoves; i++)
		Update(ml[i]);
}

// UpdateKomiSet() is called in a komi game to set the first move
//	choices of both players.
// It updates the clock of the nonmover. The mover's clock
//	is updated by a call to Update().

void COsPosition::UpdateKomiSet(const COsMoveListItem mlis[2]) {
	bool fBlackOpponent=!board.fBlackMove;
	cks[fBlackOpponent].Update(mlis[fBlackOpponent].tElapsed);
}

void COsPosition::Calculate(const COsGame& game, int nMoves) {
	(*this)=game.posStart;
	if (nMoves && game.mt.fKomi) {
        if (game.NeedsKomi()) {
            std::cerr << "[GGS] Warning: COsPosition::Calculate komi not set" << std::endl;
            return;
        }
        UpdateKomiSet(game.mlisKomi);
	}
	Update(game.ml, nMoves);
}

void COsPosition::Clear() {
	board.Clear();
	cks[0].Clear();
	cks[1].Clear();
}

bool COsGame::NeedsKomi() const {
	return mt.fKomi && ml.empty();
}

void COsGame::In(std::istream& is) {
	char c;
	std::string sToken, sData;
	bool fCheckKomiValue = false;
	double dKomiValue = 0;

	Clear();

	// Game header	fOK 

	bool fOK = (is >> c) && (c=='(') && (is >> c) && (c==';');
	if (!fOK && is)
		is.setstate(std::ios::failbit);

	if (fOK) {
		// Game tokens
		while (is >> std::ws) {
			if (is.peek()==';')
				break;
			getline(is, sToken, '[');
			getline(is, sData, ']');
			std::istringstream is(sData.c_str());

            if (sToken=="GM") {
                if (sData != "Othello")
                    std::cerr << "[GGS] Warning: COsGame::In unexpected game type='" << sData << "'" << std::endl;
            } else if (sToken=="PC")
				sPlace=sData;
			else if (sToken=="DT")
				sDateTime=sData;
			else if (sToken=="PB")
				pis[1].sName=sData;
			else if (sToken=="PW")
				pis[0].sName=sData;
			else if (sToken=="RB")
				is >> pis[1].dRating;
			else if (sToken=="RW")
				is >> pis[0].dRating;
			else if (sToken=="TI") {
				is >> posStart.cks[0];
				posStart.cks[1]=posStart.cks[0];
			}
			else if (sToken=="TB")
				is >> posStart.cks[1];
			else if (sToken=="TW")
				is >> posStart.cks[0];
			else if (sToken=="TY")
				is >> mt;
			else if (sToken=="BO")
				is >> posStart.board;
			else if (sToken=="B" || sToken=="W") {
				COsMoveListItem mli;
				is >> mli;
				ml.Update(mli);
			}
			else if (sToken=="RE")
				is >> result;
			else if (sToken=="CO")
				;	// ignore comments
			else if (sToken=="KB")
				is >> mlisKomi[1];
			else if (sToken=="KW")
				is >> mlisKomi[0];
			else if (sToken=="KM") {
				is >> dKomiValue;
				fCheckKomiValue=true;
			}
		}

		if (fCheckKomiValue) {
			double dErr = 2*dKomiValue - mlisKomi[0].dEval - mlisKomi[1].dEval;
            std::cout << dErr << std::endl;
            if (dErr >= 0.0001 || dErr <= -0.0001)
                std::cerr << "[GGS] Warning: COsGame::In komi value mismatch, err=" << dErr << std::endl;
		}

		if (is) {
			is >> c;
            if (c != ';') { std::cerr << "[GGS] Warning: COsGame::In malformed, expected ';'" << std::endl; }

			is >> c;
            if (c != ')') { std::cerr << "[GGS] Warning: COsGame::In malformed, expected ')'" << std::endl; }

		}

		// Get the current position
		CalcCurrentPos();
	}
}

std::istream& COsGame::InLogbook(std::istream& is) {
	char c;

	Clear();
	mt.bt.n=8;
	posStart.board.Initialize(mt.bt);
	pis[0].sName=pis[1].sName="logtest";
	pos=posStart;
	sPlace="logbook";

	// get moves
	COsMoveListItem mli;
	mli.tElapsed=0;
	mli.dEval=0;
	while (is >> c) {
		if (c=='+' || c=='-') {

			// read move
			//bool fBlackMove= c=='+';
			is >> mli.mv;

			// update game and pass if needed
			Update(mli);
			if (!GameOver() && !pos.board.HasLegalMove()) {
				mli.mv.fPass=true;
				Update(mli);
			}
		}
		else {
            if (c != ':') {
                std::cerr << "[GGS] Warning: COsGame::InLogbook unexpected char='" << c << "'" << std::endl;
                break;
            }
            break;
		}
	}

	// get result
	int nResult;
	is >> nResult;
    if (nResult != pos.board.NetBlackSquares())
        std::cerr << "[GGS] Warning: COsGame::InLogbook result mismatch: stored="
                  << nResult << " board=" << pos.board.NetBlackSquares() << std::endl;
    result.Set(nResult);

	// game over flag
	int n;
	is >> n;
    if (n != 10)
        std::cerr << "[GGS] Warning: COsGame::InLogbook unexpected end flag=" << n << std::endl;
    
	return is;
}

// 772942166 r idiot    64 ( 30   0   0) TravisS   0 ( 30   0   0) +34-33+43-35+24-42+52-64+23-13+41-32+53-14+25-31+51-61+15-16+63-74+62-73+65-75+66-56+76-57+67-86+46-68+47-38+26-37+58-48+36 +0
std::istream& COsGame::InIOS(std::istream& is) {
	char c;
	time_t timestamp;
	int nBlack, nWhite;

	Clear();
	mt.bt.n=8;
	posStart.board.Initialize(mt.bt);
	sPlace="IOS";

	if (is >> timestamp) {
		// time. For early games no timestamp was stored, it is 0.
		if (timestamp)
			SetTime(timestamp);

		// game end type
		is >> c;
		switch(c) {
		case 'e':
			result.status=COsResult::kNormalEnd;
			break;
		case 'r':
			result.status=COsResult::kResigned;
			break;
		case 't':
			result.status=COsResult::kTimeout;
			break;
		}

		is >> pis[1].sName;
		is >> nBlack;
		posStart.cks[1].InIOS(is);

		is >> pis[0].sName;
		is >> nWhite;
		posStart.cks[0].InIOS(is);

		pos=posStart;

		// get moves
		COsMoveListItem mli;
		mli.tElapsed=0;
		mli.dEval=0;
		int iosmove;

		// read move code. move code 0 means game is over
		while ((is >> iosmove) && iosmove) {
			// positive moves are black, negative are white
            if (pos.board.fBlackMove != (iosmove > 0))
                std::cerr << "[GGS] Warning: COsGame::InIOS move color mismatch" << std::endl;
            
			mli.mv.SetIOS(iosmove);
			Update(mli);

			// pass if needed
			if (!GameOver() && !pos.board.HasLegalMove()) {
				mli.mv.fPass=true;
				Update(mli);
			}
		}

		// calculate result. Might not be equal to the result
		//	on the board if one player resigned.
		result.dResult=nBlack-nWhite;
        if (result.dResult != pos.board.NetBlackSquares() && result.status == COsResult::kNormalEnd)
            std::cerr << "[GGS] Warning: COsGame::InIOS result mismatch: stored="
                      << result.dResult << " board=" << pos.board.NetBlackSquares() << std::endl;
	}

	return is;
}

void COsGame::Out(std::ostream& os) const {
	os << "(;GM[Othello]";
	os << "PC[" << sPlace;
	if (!sDateTime.empty())
		os << "]DT[" << sDateTime;
	os << "]PB[" << pis[1].sName;
	os << "]PW[" << pis[0].sName;
	os << "]RE[" << result;

	if (pis[1].dRating!=0)
		os << "]RB[" << pis[1].dRating;
	if (pis[0].dRating!=0)
		os << "]RW[" << pis[0].dRating;
	if (posStart.cks[0]==posStart.cks[1]) {
		os << "]TI[" << posStart.cks[1];
	}
	else {
		os << "]TB[" << posStart.cks[1];
		os << "]TW[" << posStart.cks[0];
	}
	os << "]TY[" << mt;
	os << "]BO[" << posStart.board;
	
	// komi set moves
	if (mt.fKomi && !ml.empty()) {
		os << "]KB[" << mlisKomi[1];
		os << "]KW[" << mlisKomi[0];
		os << "]KM[" << (mlisKomi[0].dEval + mlisKomi[1].dEval)/2;
	}

	// move list
	std::vector<COsMoveListItem>::const_iterator pmli;
	bool fBlackMove=posStart.board.fBlackMove;
	for (pmli=ml.begin(); pmli!=ml.end(); pmli++) {
		os << (fBlackMove?"]B[":"]W[") << *pmli;
		fBlackMove=!fBlackMove;
	}

	os << "];)";
}

void COsGame::Clear() {
	result.Clear();
	ml.clear();
	pis[0].Clear();
	pis[1].Clear();
	pos.Clear();
	posStart.Clear();
	sDateTime.erase();
	sPlace.erase();
	mt.Clear();
}

void COsGame::Undo() {
	if (ml.size()>=2) {
		ml.resize(ml.size()-2);
		CalcCurrentPos();
	}
}

void COsGame::SetResult(const COsResult& aresult, const COsPlayerInfo apis[2]) {
	result=aresult;
	if (pis[0].sName!=apis[0].sName)
		result.dResult=-result.dResult;
}

void COsGame::SetResult(const COsResult& aresult, const std::string sNames[2]) {
	result=aresult;
	if (pis[0].sName!=sNames[0])
		result.dResult=-result.dResult;
}

void COsGame::SetTime(time_t t) {
	// sDateTime
	char sTime[30];
	strftime(sTime, 30, "%Y-%m-%d %H:%M:%S GMT", gmtime(&t));
	sDateTime=sTime;
}

void COsGame::SetCurrentTime() {
	time_t tCurrent=time(nullptr);
	SetTime(tCurrent);
}

void COsGame::CalcCurrentPos() {
	std::vector<COsMoveListItem>::const_iterator pmli;

	pos=posStart;
	if (mt.fKomi && !ml.empty())
		pos.UpdateKomiSet(mlisKomi);

	for (pmli=ml.begin(); pmli!=ml.end(); pmli++) 
		pos.Update(*pmli);
}

void COsGame::Update(const COsMoveListItem& mli) {
	pos.Update(mli);
	ml.Update(mli);
	if (GameOver()) {
		// we don't adjust for timeouts because we don't keep track of
		//	who timed out first. This is a bug; but in games coming from GGS
		//	it should update us with the final result later.
		result.dResult=pos.board.Result(mt.fAnti);
		result.status=COsResult::kNormalEnd;
	}
}

inline double max(double a, double b) {
	return (a>b)?a:b;
}

void COsGame::UpdateKomiSet(const COsMoveListItem mlis[2]) {
	if (NeedsKomi()) {
		pos.UpdateKomiSet(mlis);
		mlisKomi[0]=mlis[0];
		mlisKomi[1]=mlis[1];
	}
}

bool COsGame::GameOver() const {
	return pos.board.GameOver();
}

bool COsGame::ToMove(const std::string& sLogin) const {
	if (result.status!=COsResult::kUnfinished)
		return false;
	if (pos.board.GameOver())
		return false;
	if (NeedsKomi())
		return sLogin==pis[0].sName || sLogin==pis[1].sName;
	else
		return sLogin==pis[pos.board.fBlackMove].sName;
}

/////////////////////////////////////////////
// COsHistoryItem
/////////////////////////////////////////////

// .48723   25 Mar 2001 03:28:00 1780 nasai    2144 OO7        -4.0 8

void COsHistoryItem::In(std::istream& is) {
	is >> idsm >> dt >> pis[0] >> pis[1] >> result >> mt;
}

/////////////////////////////////////////////
// COsMatch
/////////////////////////////////////////////

std::istream& COsMatch::InDelta(std::istream& is) {
	char c;
	is >> idm >> pis[1] >> pis[0] >> mt >> c;
	fRated= c=='R' || c=='S';
	nObservers=0;

	return is;
}

// |  .9   s8r20  R 2574 lynx     2570 kitty    2
std::istream& COsMatch::In(std::istream& is) {
	char c;

	is >> idm >> mt >> c >> pis[1] >> pis[0] >> nObservers;
	fRated= c=='R' || c=='S';

	return is;
}

bool COsMatch::IsPlaying(const std::string& sLogin) const {
	return pis[0].sName==sLogin || pis[1].sName==sLogin;
}

void COsPlayerInfo::In(std::istream& is) {
	is >> dRating >> sName;
}

void COsPlayerInfo::Clear() {
	dRating=0;
	sName.erase();
}

void COsRating::In(std::istream& is) {
	char c;
	is >> dRating >> c >> dSD;
    if (c != '@')
        std::cerr << "[GGS] Warning: COsRating malformed, expected '@', got '" << c << "'" << std::endl;
}

double COsRating::AdjustedRating() const {
	return dRating-dSD*(1720.0/350.0);
}

//     1 leaflet  2103.9@ 33.8=   1.07:08:24+@ 31.3  +4.5    551     65    395
void COsRankData::In(std::istream& is) {
	std::string s;
	std::string sLine;

	getline(is, sLine);
	if (!sLine.empty()) {
		std::istringstream isLine(sLine.c_str());

		isLine  >> iRank >> sLogin;
		isLine >> rd;
		isLine >> s;
		fMe=s=="<=";
	}
}

void COsRatingData::In(std::istream& is) {
	char c;

	if ( is  >> rating >> c) {
        if (c != '=') { std::cerr << "[GGS] Warning: COsRatingData malformed, expected '='" << std::endl; return; }

		sInactive="hello";
		getline(is, sInactive, char('+'));
		is >> c;
		is >> d1 >> d2 >> nWins >> nDraws >> nLosses;
	}
}

// 1735.1 pamphlet 15:00//02:00        8 U 1345.6 ant
void COsRequest::In(std::istream& is) {
	char c;
	is >> pis[0] >> cks[0];
	is >> mt >> c;
	is >> pis[1] >> cks[1];
	if (!is.good())
		cks[1]=cks[0];
	fRated= c=='R' || c=='S';
}

// "nasai left" in matchdelta messages means game adjourned
//	"?" in other messages
void COsResult::In(std::istream& is) {
	is >> std::ws;

	char c;
	c=is.peek();
	if (c=='?') {
		status=kUnfinished;
		dResult=0;
	}
	else if (isalpha(c)) {
		dResult=0;
		std::string sResult;
		is >> sResult;
		if (sResult=="aborted")
			status=kAborted;
		else
			status=kAdjourned;
	}
	else {
		is >> dResult;
		status=kNormalEnd;
		c=is.peek();
		if (c==':') {
			is >> c >> c;
			switch(c) {
			case 'r': status=kResigned; break;
			case 't': status=kTimeout; break;
			case 'l': status=kAdjourned; break;
			}
		}
	}
}

void COsResult::Out(std::ostream& os) const {
	if (status==kUnfinished || status==kAdjourned)
		os << '?';
	else {
		os << dResult;
		switch(status) {
		case kResigned:
			os << ":r";
			break;
		case kTimeout:
			os << ":t";
			break;
		case kNormalEnd:
			break;
        case kUnfinished:
            break;
        case kAgreedScore:
            break;
        case kAdjourned:
            break;
        case kAborted:
            break;

		}
	}
}

void COsResult::Set(double adResult, TStatus astatus) {
	dResult=adResult;
	status=astatus;
}

void COsResult::Clear() {
	status=kUnfinished;
	dResult=0;
}

void COsFingerRating ::In(std::istream& is) {
	is >> rt >> rd;
}

// |.42590   23 Feb 2001 23:17:39 pamphlet patzer   s8r16:l
void COsStoredMatch::In(std::istream& is) {
	std::string s;

	is >> idsm >> dt >> sPlayers[0] >> sPlayers[1] >> mt >> s;
    
    if (is)
        if (is && s != ":l")
            std::cerr << "[GGS] Warning: COsStoredMatch unexpected suffix='" << s << "'" << std::endl;
}

/*
/os: who    26      change:    win    draw    loss         match(es)
|tit4tat  + 1145.7@301.5 ->   +14.3   -12.1   -38.4 @ 79.6 
|n2       + 1384.1@ 80.5 
|ant      + 1426.2@ 50.8 ->   +39.2    +4.2   -30.8 @ 78.5 
etc.
*/

void COsWhoItem::In(std::istream& isAll) {
	std::string sLine;
	getline(isAll, sLine);

	if (!sLine.empty()) {
		std::istringstream is(sLine.c_str());
		std::string s;
		char c;

		is >> sLogin >> c >> rating >> s;
        if (c != '+') { std::cerr << "[GGS] Warning: COsWhoItem malformed, expected '+'" << std::endl; return; }

		fMe=!is;
		if (!fMe) {
            if (s != "->") { std::cerr << "[GGS] Warning: COsWhoItem malformed, expected '->'" << std::endl; return; }
			is >> dWin >> dDraw >> dLoss >> c >> dSDNew;
            if (c != '@') { std::cerr << "[GGS] Warning: COsWhoItem malformed, expected '@'" << std::endl; return; }
		}
	}
}

bool COsWhoItem::operator<(const COsWhoItem& b) const {
	double ar=rating.AdjustedRating(), br=b.rating.AdjustedRating();
	if (ar!=br)
		return ar<br;
	else
		return sLogin<b.sLogin;
}

bool COsWhoItem::operator==(const COsWhoItem& b) const {
	return sLogin==b.sLogin;
}
