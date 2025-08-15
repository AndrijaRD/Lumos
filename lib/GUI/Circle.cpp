#include "gui.h"
#include "../System/Sys.h"

void GUI::Circle(
    const SDL_Point& center, 
    const int& radius, 
    const SDL_Color& color, 
    const int& thickness
) {
    SDL_SetRenderDrawColor(Sys::renderer, color.r, color.g, color.b, color.a);

    // Choose an appropriate number of segments. More segments produce a smoother circle.
    // Here we use at least 32 segments, or more if the circle is large.
    int segments = std::max(32, radius);

    if (thickness == -1) {
        // FILLED CIRCLE: Use a triangle fan.
        std::vector<SDL_Vertex> vertices;
        std::vector<int> indices;

        // Center vertex:
        SDL_Vertex centerV;
        centerV.position = { static_cast<float>(center.x), static_cast<float>(center.y) };
        centerV.color    = TO_FCOLOR(color);
        centerV.tex_coord= { 0.f, 0.f };
        vertices.push_back(centerV);

        float angleStep = 2.0f * M_PI / segments;
        // Generate perimeter vertices:
        for (int i = 0; i <= segments; i++) {
            float angle = i * angleStep;
            SDL_Vertex v;
            v.position = { center.x + radius * cos(angle),
                           center.y + radius * sin(angle) };
            v.color    = TO_FCOLOR(color);
            v.tex_coord= { 0.f, 0.f };
            vertices.push_back(v);
        }

        // Create indices for the triangle fan.
        // Each triangle is: center, vertex[i], vertex[i+1]
        for (int i = 1; i <= segments; i++) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        SDL_RenderGeometry(Sys::renderer, nullptr,
                           vertices.data(), static_cast<int>(vertices.size()),
                           indices.data(), static_cast<int>(indices.size()));
    }
    else {
        // OUTLINED (RING) MODE: Create an outer circle and an inner circle.
        // Clamp thickness to be at most the radius.
        int t = (thickness >= radius) ? radius : thickness;

        std::vector<SDL_Vertex> outerVerts;
        std::vector<SDL_Vertex> innerVerts;
        float angleStep = 2.0f * M_PI / segments;
        for (int i = 0; i <= segments; i++) {
            float angle = i * angleStep;

            SDL_Vertex ov;
            ov.position = { center.x + radius * cos(angle),
                            center.y + radius * sin(angle) };
            ov.color    = TO_FCOLOR(color);
            ov.tex_coord= { 0.f, 0.f };
            outerVerts.push_back(ov);

            SDL_Vertex iv;
            iv.position = { center.x + (radius - t) * cos(angle),
                            center.y + (radius - t) * sin(angle) };
            iv.color    = TO_FCOLOR(color);
            iv.tex_coord= { 0.f, 0.f };
            innerVerts.push_back(iv);
        }

        // Stitch the outer and inner circles together.
        std::vector<SDL_Vertex> vertices;
        std::vector<int> indices;
        // For each segment, form a quad between outerVerts[i] to outerVerts[i+1] and innerVerts[i] to innerVerts[i+1]
        for (int i = 0; i < segments; i++) {
            int base = static_cast<int>(vertices.size());
            vertices.push_back(outerVerts[i]);
            vertices.push_back(outerVerts[i + 1]);
            vertices.push_back(innerVerts[i + 1]);
            vertices.push_back(innerVerts[i]);

            // Two triangles per quad.
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);

            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        }

        SDL_RenderGeometry(Sys::renderer, nullptr,
                           vertices.data(), static_cast<int>(vertices.size()),
                           indices.data(), static_cast<int>(indices.size()));
    }
}