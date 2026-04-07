// Copyleft 2001 Chris Welty
//	All Rights Reserved

#ifndef ODK_ODKSTREAM_HPP
#define ODK_ODKSTREAM_HPP

#include "ggsstream.hpp"

class RXRoxane;

class CODKStream: public ggsstream {
public:

	virtual void HandleGGS				(const CMsg* pmsg);
	virtual void HandleGGSLogin			();
	virtual void HandleGGSTell			(const CMsgGGSTell* pmsg);
	virtual void HandleGGSUnknown		(const CMsgGGSUnknown* pmsg);

	virtual void HandleOsJoin			(const CMsgOsJoin* pmsg);
	virtual void HandleOsLogin			();
	virtual void HandleOsUnknown		(const CMsgOsUnknown* pmsg);
	virtual void HandleOsUpdate			(const CMsgOsUpdate* pmsg);

    virtual void HandleOsEnd            (const CMsgOsEnd* pmsg);
    virtual void HandleOsTimeout        (const CMsgOsTimeout* pmsg);
    virtual void HandleOsFatalTimeout   (const CMsgOsFatalTimeout* pmsg);
    virtual void HandleOsRequestDelta   (const CMsgOsRequestDelta* pmsg);
    virtual void HandleOsGameOver       (const CMsgOsMatchDelta* pmsg, const std::string& idg);

    virtual void HandleOsMatchDelta     (const CMsgOsMatchDelta* pmsg);


	virtual void GetMoveIfNeeded        (const std::string& idg);
	virtual void SendMove               (const std::string& idg, COsMoveListItem& mli);
	virtual void SendMsg                (const std::string& msg);
    


	
	RXRoxane* pComputer;
};

#endif // ODK_GGSSTREAM_HPP
