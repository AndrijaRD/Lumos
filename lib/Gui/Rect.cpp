#include "gui.h"
#include "../System/Sys.h"
#include <cstdlib>

// A helper function that always produces (segments+1) vertices per corner,
// even if the radius is 0 (in which case it outputs duplicate vertices).
static std::vector<SDL_Vertex> computeRoundedRectPolygonFixed(
    const SDL_Rect& rect,
    const BorderRadiusRect& br,
    const SDL_Color& color,
    int segments = 8)
{

    std::vector<SDL_Vertex> vertices;
    auto pushVertex = [&](float x, float y) {
        SDL_Vertex v;
        v.position = { x, y };
        v.color    = color;
        v.tex_coord = { 0.f, 0.f };
        vertices.push_back(v);
    };

    // Helper: compute an arc for one corner.
    auto computeCorner = [&](float cx, float cy, float startAngle, float endAngle, unsigned int r) -> std::vector<SDL_FPoint> {
        std::vector<SDL_FPoint> pts;
        // Always output segments+1 vertices.
        for (int i = 0; i <= segments; i++) {
            float t = static_cast<float>(i) / segments;
            float angle = (startAngle + t * (endAngle - startAngle)) * (M_PI / 180.f);
            // If r==0, this will always yield the same point.
            float rad = (r > 0) ? r : 0;
            pts.push_back({ cx + rad * cos(angle), cy + rad * sin(angle) });
        }
        return pts;
    };

    // Compute arcs for each corner:
    // Top-left: center is at (rect.x + br.top_left, rect.y + br.top_left)
    std::vector<SDL_FPoint> arcTL = computeCorner(rect.x + br.top_left, rect.y + br.top_left, 180.f, 270.f, br.top_left);
    // Top-right: center is at (rect.x + rect.w - br.top_right, rect.y + br.top_right)
    std::vector<SDL_FPoint> arcTR = computeCorner(rect.x + rect.w - br.top_right, rect.y + br.top_right, 270.f, 360.f, br.top_right);
    // Bottom-right: center is at (rect.x + rect.w - br.bottom_right, rect.y + rect.h - br.bottom_right)
    std::vector<SDL_FPoint> arcBR = computeCorner(rect.x + rect.w - br.bottom_right, rect.y + rect.h - br.bottom_right, 0.f, 90.f, br.bottom_right);
    // Bottom-left: center is at (rect.x + br.bottom_left, rect.y + rect.h - br.bottom_left)
    std::vector<SDL_FPoint> arcBL = computeCorner(rect.x + br.bottom_left, rect.y + rect.h - br.bottom_left, 90.f, 180.f, br.bottom_left);

    // Now build the final polygon.
    // We also add one extra vertex between corners (i.e. along the straight edges)
    // so that both the outer and inner polygons have matching vertex counts.
    for (auto &pt : arcTL) {
        pushVertex(pt.x, pt.y);
    }
    // Top edge: use arcTR's first point
    pushVertex(arcTR.front().x, arcTR.front().y);
    for (size_t i = 1; i < arcTR.size(); i++) {
        pushVertex(arcTR[i].x, arcTR[i].y);
    }
    // Right edge: use arcBR's first point
    pushVertex(arcBR.front().x, arcBR.front().y);
    for (size_t i = 1; i < arcBR.size(); i++) {
        pushVertex(arcBR[i].x, arcBR[i].y);
    }
    // Bottom edge: use arcBL's first point
    pushVertex(arcBL.front().x, arcBL.front().y);
    for (size_t i = 1; i < arcBL.size(); i++) {
        pushVertex(arcBL[i].x, arcBL[i].y);
    }
    // Left edge: close the polygon with arcTL's first point
    pushVertex(arcTL.front().x, arcTL.front().y);

    return vertices;
}


/** Rect
 * 
 * This function draws a Rectangle with custom:
 *   - Color
 *   - Thickness
 *   - Border Radius
 * To change the Border Radius use GUI::pushBorderRadius();
 * 
 * @param dRect the position and size of the rect
 * @param color the rects color
 * @param thickness if its -1 then it will be filled rect, otherwise outline
 */
// Main function for drawing a rectangle with custom thickness and border radius.
void GUI::Rect(
    const SDL_Rect& dRect, 
    const SDL_Color& color, 
    const int thickness
) {
    BorderRadiusRect borderRadius = pBorderRadius;
    pBorderRadius = {0, 0, 0, 0};

    // Check if border radius per side is bigger then the side it self
    // example i set border radius to be 30px and height of the rect is 40px
    // then the max allowed border radius per corner should be 20px
    
    uint rectW = static_cast<unsigned int>(std::abs(dRect.w));
    uint rectH = static_cast<unsigned int>(std::abs(dRect.h));
    if(borderRadius.top_left + borderRadius.bottom_left > rectH){
        borderRadius.top_left = rectH/2;
        borderRadius.bottom_left = rectH/2;
    }

    if(borderRadius.top_right + borderRadius.bottom_right > rectH){
        borderRadius.top_right = rectH/2;
        borderRadius.bottom_right = rectH/2;
    }

    if(borderRadius.top_left + borderRadius.top_right > rectW){
        borderRadius.top_left = rectW/2;
        borderRadius.top_right = rectW/2;
    }

    if(borderRadius.bottom_left + borderRadius.bottom_right > rectW){
        borderRadius.bottom_left = rectW/2;
        borderRadius.bottom_right = rectW/2;
    }

    // If no rounded corners are requested, use SDL’s routines.
    bool noRounded = (borderRadius.top_left == 0 && borderRadius.top_right == 0 &&
                      borderRadius.bottom_left == 0 && borderRadius.bottom_right == 0);

    SDL_SetRenderDrawColor(Sys::renderer, color.r, color.g, color.b, color.a);
    
    if (noRounded) {
        if (thickness == -1) {
            SDL_RenderFillRect(Sys::renderer, &dRect);
        }
        else if (thickness == 1) {
            SDL_RenderDrawRect(Sys::renderer, &dRect);
        }
        else {
            for (int i = 0; i < thickness; i++) {
                SDL_Rect r = { dRect.x + i, dRect.y + i, dRect.w - 2 * i, dRect.h - 2 * i };
                SDL_RenderDrawRect(Sys::renderer, &r);
            }
        }
        return;
    }

    const int arcSegments = 8;  // Adjust for smoother arcs if desired.

    if (thickness == -1) {
        // Filled rounded rectangle.
        std::vector<SDL_Vertex> outerVerts = computeRoundedRectPolygonFixed(dRect, borderRadius, color, arcSegments);

        // Create a fan by choosing the center of the rect.
        SDL_FPoint center = { dRect.x + dRect.w / 2.f, dRect.y + dRect.h / 2.f };
        SDL_Vertex centerV;
        centerV.position = { center.x, center.y };
        centerV.color    = color;
        centerV.tex_coord = { 0.f, 0.f };

        std::vector<SDL_Vertex> fanVerts;
        fanVerts.push_back(centerV);
        for (const auto &v : outerVerts)
            fanVerts.push_back(v);
        // Close the fan.
        fanVerts.push_back(outerVerts.front());

        std::vector<int> indices;
        int n = static_cast<int>(fanVerts.size());
        for (int i = 1; i < n - 1; i++) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
        SDL_RenderGeometry(Sys::renderer, nullptr,
                           fanVerts.data(), static_cast<int>(fanVerts.size()),
                           indices.data(), static_cast<int>(indices.size()));
    }
    else {
        // Outlined (stroked) mode with custom thickness.
        // Outer polygon (using full radii).
        std::vector<SDL_Vertex> outerVerts = computeRoundedRectPolygonFixed(dRect, borderRadius, color, arcSegments);

        // Define inner rect inset by thickness.
        SDL_Rect innerRect = { dRect.x + thickness, dRect.y + thickness,
                               dRect.w - 2 * thickness, dRect.h - 2 * thickness };

        // For inner radii, subtract thickness but clamp at 0.
        BorderRadiusRect innerBR;
        innerBR.top_left     = (borderRadius.top_left     > (unsigned)thickness) ? borderRadius.top_left     - thickness : 0;
        innerBR.top_right    = (borderRadius.top_right    > (unsigned)thickness) ? borderRadius.top_right    - thickness : 0;
        innerBR.bottom_right = (borderRadius.bottom_right > (unsigned)thickness) ? borderRadius.bottom_right - thickness : 0;
        innerBR.bottom_left  = (borderRadius.bottom_left  > (unsigned)thickness) ? borderRadius.bottom_left  - thickness : 0;

        // Using our fixed polygon generator ensures that even when a radius becomes 0,
        // the inner polygon has the same vertex count as the outer polygon.
        std::vector<SDL_Vertex> innerVerts = computeRoundedRectPolygonFixed(innerRect, innerBR, color, arcSegments);

        // Stitch the outer and inner polygons together.
        size_t n = outerVerts.size();
        std::vector<SDL_Vertex> borderVerts;
        std::vector<int> indices;
        for (size_t i = 0; i < n; i++) {
            size_t next = (i + 1) % n;
            int base = static_cast<int>(borderVerts.size());
            borderVerts.push_back(outerVerts[i]);
            borderVerts.push_back(outerVerts[next]);
            borderVerts.push_back(innerVerts[next]);
            borderVerts.push_back(innerVerts[i]);

            // Two triangles per quad.
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);

            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        }
        SDL_RenderGeometry(Sys::renderer, nullptr,
                           borderVerts.data(), static_cast<int>(borderVerts.size()),
                           indices.data(), static_cast<int>(indices.size()));
    }
}