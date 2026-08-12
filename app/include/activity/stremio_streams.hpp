/*
    Stremio stream picker

    A scrollable, selectable list of streams for a chosen movie/episode. Built
    on the same RecyclingGrid that the rest of the app uses (so it works with
    both touch and the D-pad). Selecting a row plays that stream.
*/
#pragma once

#include <borealis.hpp>
#include "api/stremio.hpp"
#include "activity/stremio_resume.hpp"

class RecyclingGrid;
class ActionBar;

class StreamPicker : public brls::Box {
public:
    StreamPicker(const std::string& title, const std::vector<stremio::Stream>& streams, const ResumeEntry& resumeKey);

private:
    void applyStreamView();
    void updateActionBar();

    RecyclingGrid* recycler = nullptr;
    ActionBar* actionBar = nullptr;
    ResumeEntry resumeKey;
    std::vector<stremio::Stream> allStreams;
    bool hdOnly = false;
    bool sortByQuality = false;
};
