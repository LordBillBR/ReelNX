/*
    Stremio library screen

    Full-screen grid of everything saved to the Library (the old favourites),
    with search (filter by title) and sort (recent / name / year / rating)
    controls at the top. A opens the title, X removes it from the library.
*/
#pragma once

#include <borealis.hpp>
#include "api/stremio.hpp"

class RecyclingGrid;

class StremioLibrary : public brls::Box {
public:
    StremioLibrary();
    ~StremioLibrary() override;

    // Paints the shared ocean-gradient background.
    void draw(NVGcontext* vg, float x, float y, float width, float height, brls::Style style,
        brls::FrameContext* ctx) override;

private:
    void rebuild();  // re-filter + re-sort + reload the grid

    brls::Label* headline = nullptr;
    brls::Label* lblSearch = nullptr;
    brls::Label* lblSort = nullptr;
    brls::Box* btnSearch = nullptr;
    brls::Box* btnSort = nullptr;
    RecyclingGrid* recycler = nullptr;

    int sortMode = 0;    // 0 recent, 1 name, 2 year, 3 rating
    std::string filter;  // case-insensitive title substring
    brls::VoidEvent::Subscription changedSub;
};
