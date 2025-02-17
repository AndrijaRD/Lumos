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
void GUI::Line(
    const SDL_Point& p1,
    const SDL_Point& p2,
    const SDL_Color& color,
    const uint& thickness
) {
    // For a thickness of 1, use the default line drawing.
    if (thickness <= 1) {
        SDL_SetRenderDrawColor(Sys::renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLine(Sys::renderer, p1.x, p1.y, p2.x, p2.y);
        return;
    }
    
    // Compute the directional vector from p1 to p2.
    float dx = static_cast<float>(p2.x - p1.x);
    float dy = static_cast<float>(p2.y - p1.y);
    float length = std::sqrt(dx * dx + dy * dy);
    
    // Avoid division by zero.
    if (length == 0) {
        return;
    }
    
    // Calculate the unit perpendicular vector.
    float udx = -dy / length;
    float udy = dx / length;
    
    // Scale by half the thickness.
    float offsetX = udx * (thickness / 2.0f);
    float offsetY = udy * (thickness / 2.0f);
    
    // Calculate the four corners of the rectangle.
    SDL_FPoint p1p = { p1.x + offsetX, p1.y + offsetY };
    SDL_FPoint p2p = { p2.x + offsetX, p2.y + offsetY };
    SDL_FPoint p2m = { p2.x - offsetX, p2.y - offsetY };
    SDL_FPoint p1m = { p1.x - offsetX, p1.y - offsetY };
    
    // Create vertices for the rectangle (as two triangles).
    SDL_Vertex vertices[4];
    
    vertices[0].position = { p1p.x, p1p.y };
    vertices[0].color    = color;
    vertices[0].tex_coord = { 0.f, 0.f };

    vertices[1].position = { p2p.x, p2p.y };
    vertices[1].color    = color;
    vertices[1].tex_coord = { 0.f, 0.f };

    vertices[2].position = { p2m.x, p2m.y };
    vertices[2].color    = color;
    vertices[2].tex_coord = { 0.f, 0.f };

    vertices[3].position = { p1m.x, p1m.y };
    vertices[3].color    = color;
    vertices[3].tex_coord = { 0.f, 0.f };

    // Define the indices for two triangles (0-1-2 and 0-2-3).
    int indices[6] = { 0, 1, 2, 0, 2, 3 };

    // Render the geometry.
    SDL_RenderGeometry(Sys::renderer, nullptr, vertices, 4, indices, 6);
}

