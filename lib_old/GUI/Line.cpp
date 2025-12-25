#include "gui.h"
#include "../System/Sys.h"


/** Line
 * 
 * Draws a 1px thick line from p1 to p2 with custom color
 * 
 * @param p1 SDL_Point line start coordinates
 * @param p2 SDL_Point line end coordinates
 * @param color SDL_Color line color
 */

// Helper: Draw a thick line segment (from p1 to p2) using the rectangle method.
// This is essentially the same as your original geometry code.
void drawThickLineSegment(const SDL_Point& p1, const SDL_Point& p2, const SDL_Color& color, int thickness) {
    // For a thickness of 1, we can simply draw a normal line.
    if (thickness <= 1) {
        SDL_SetRenderDrawColor(Sys::renderer, color);
        SDL_RenderLine(Sys::renderer, p1.x, p1.y, p2.x, p2.y);
        return;
    }

    // Compute vector from p1 to p2.
    float dx = static_cast<float>(p2.x - p1.x);
    float dy = static_cast<float>(p2.y - p1.y);
    float length = std::sqrt(dx * dx + dy * dy);
    const double EPSILON = 1e-6;
    if (std::abs(length) < EPSILON)
        return;

    // Unit perpendicular vector (rotated 90° clockwise).
    float udx = -dy / length;
    float udy = dx / length;

    // Half-thickness offset.
    float offsetX = udx * (thickness / 2.0f);
    float offsetY = udy * (thickness / 2.0f);

    // Compute rectangle corners.
    SDL_FPoint p1p = { p1.x + offsetX, p1.y + offsetY };
    SDL_FPoint p2p = { p2.x + offsetX, p2.y + offsetY };
    SDL_FPoint p2m = { p2.x - offsetX, p2.y - offsetY };
    SDL_FPoint p1m = { p1.x - offsetX, p1.y - offsetY };

    SDL_Vertex vertices[4];
    vertices[0].position = { p1p.x, p1p.y };
    vertices[0].color    = TO_FCOLOR(color);
    vertices[0].tex_coord = { 0.f, 0.f };

    vertices[1].position = { p2p.x, p2p.y };
    vertices[1].color    = TO_FCOLOR(color);
    vertices[1].tex_coord = { 0.f, 0.f };

    vertices[2].position = { p2m.x, p2m.y };
    vertices[2].color    = TO_FCOLOR(color);
    vertices[2].tex_coord = { 0.f, 0.f };

    vertices[3].position = { p1m.x, p1m.y };
    vertices[3].color    = TO_FCOLOR(color);
    vertices[3].tex_coord = { 0.f, 0.f };

    int indices[6] = { 0, 1, 2, 0, 2, 3 };
    SDL_RenderGeometry(Sys::renderer, nullptr, vertices, 4, indices, 6);
}


// Main function to draw a line, which supports both solid and dashed styles.
void GUI::Line(
    const SDL_Point& p1,
    const SDL_Point& p2,
    const SDL_Color& color,
    const uint& thickness
) {
    if (!pDashLine) {
        // Use the existing solid-line code (or call drawThickLineSegment once).
        drawThickLineSegment(p1, p2, color, thickness);
        return;
    }
    pDashLine = false;

    // Dashed line mode.
    // Compute full line vector.
    float dx = static_cast<float>(p2.x - p1.x);
    float dy = static_cast<float>(p2.y - p1.y);
    float totalLength = std::sqrt(dx * dx + dy * dy);
    const double EPSILON = 1e-6;
    if (std::abs(totalLength) < EPSILON) {
        return;
    }
    
    // Compute unit vector in direction of line.
    float ux = dx / totalLength;
    float uy = dy / totalLength;

    // Iterate along the line, drawing dash segments.
    float currentDistance = 0.0f;
    while (currentDistance < totalLength) {
        // Determine the dash segment: start at currentDistance, dash until either dashSize or the end.
        float dashStart = currentDistance;
        float dashEnd = std::min(currentDistance + pDashSize, totalLength);
        
        // Compute dash segment endpoints.
        SDL_Point dashP1 = {
            static_cast<int>(p1.x + ux * dashStart),
            static_cast<int>(p1.y + uy * dashStart)
        };
        SDL_Point dashP2 = {
            static_cast<int>(p1.x + ux * dashEnd),
            static_cast<int>(p1.y + uy * dashEnd)
        };

        // Draw this dash segment.
        drawThickLineSegment(dashP1, dashP2, color, thickness);

        // Skip the gap segment.
        currentDistance += pDashSize + pDashGapSize;
    }
}