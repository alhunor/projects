#ifndef _REPOSITORY_H_
#define _REPOSITORY_H_

#include <set>
#include <iostream>
#include "utils.h"

class repository
{
public:
	repository(): nbItems(0), noCopy(false) {}
	~repository();

	void commitDirectory(boost::shared_ptr<wchar_t> active);
	void loadDirectory(boost::shared_ptr<wchar_t> path);
	bool cleanRepository(bool recursive);
	void setNoCopy(bool b) {noCopy=b;}
	bool getNoCopy() {return noCopy;}

//	void RebuildFromDisk(wchar_t* path);
	bool addFile(fileInfoT* info);
	bool getInfo(wchar_t* path);
	void listItems(wchar_t* path);
//	bool commitDelete(fileInfoT* info);
//	bool commitRename(fileInfoT* info, fileInfoT* info2);
	void setRoot(boost::shared_ptr<wchar_t> _sourceroot, boost::shared_ptr<wchar_t> _reporoot) {sourceroot=_sourceroot; reporoot=_reporoot;};
	boost::shared_ptr<wchar_t> sourceroot;
	boost::shared_ptr<wchar_t> reporoot;

protected:
	void LoadInfoFile(boost::shared_ptr<wchar_t> path, boost::shared_ptr<wchar_t> active);
	void SaveInfoFile(boost::shared_ptr<wchar_t> path);

	int nbItems;
	bool noCopy;
//	std::list<fileInfoT*> items;
	std::set<fileInfoT*, fileInfoT::less> itemset;
	friend std::ostream& operator<<(std::ostream& o, repository& rep);
};


#endif