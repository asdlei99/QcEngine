#pragma once


class QcMediaFileFinder : public QcFileFinder
{
public:
	explicit QcMediaFileFinder();

	void Reset();
private:
	bool SearchMediaPath(const QcString& modulePath, QcString& folder);
};

#define QcMediaFileFinderInstance QmGlobalData.gpMediaFinder