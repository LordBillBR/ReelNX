/*
    Stremio series screens

    StremioSeries: fetches a series' metadata and lists its SEASONS.
    StremioSeason: lists the EPISODES of one season; selecting an episode
                   fetches its streams and opens the picker.
*/
#pragma once

#include <borealis.hpp>
#include "api/stremio.hpp"

class RecyclingGrid;
class ActionBar;

class StremioSeries : public brls::Box {
public:
    // Fetches the series meta from Cinemeta, then lists its seasons.
    explicit StremioSeries(const stremio::Meta& series);
    // Uses already-fetched episodes (e.g. from the detail screen) — no refetch.
    // background: series backdrop, used when an episode thumbnail is missing.
    StremioSeries(const std::string& name, const std::vector<stremio::Video>& videos, const std::string& background);

private:
    void init();
    void setSeasons(
        const std::string& name, const std::vector<stremio::Video>& videos, const std::string& background);

    RecyclingGrid* recycler = nullptr;
    ActionBar* actionBar = nullptr;
};

class StremioSeason : public brls::Box {
public:
    StremioSeason(const std::string& seriesName, const std::string& title,
        const std::vector<stremio::Video>& episodes, const std::string& background);

private:
    RecyclingGrid* recycler = nullptr;
    ActionBar* actionBar = nullptr;
};
