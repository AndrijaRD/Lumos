#include "gui.h"
#include "../System/Sys.h"
#include <cstdlib>





/**
 * Draws a filled rounded rectangle.
 *
 * @param renderer     Your SDL_Renderer*.
 * @param rect         The rectangle (x,y,w,h).
 * @param radius       Corner radius in pixels (clamped to half-width/height).
 * @param color        Fill color.
 * @param segments     How many segments per corner (8–16 is usually plenty).
 */
void drawRoundedFilledRect(
    SDL_Rect         rect,
    unsigned int     radius,
    SDL_Color        color,
    int              segments = 12
) {
    // Clamp radius so corners never overlap
    radius = std::min<unsigned>(radius, rect.w/2u);
    radius = std::min<unsigned>(radius, rect.h/2u);

    // Precompute normalized color
    SDL_FColor fcol = {
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    };

    // 1) Fill center rectangle
    SDL_FRect r = {
        float(rect.x + radius),
        float(rect.y),
        float(rect.w - 2*radius),
        float(rect.h)
    };
    SDL_SetRenderDrawColor(Sys::renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(Sys::renderer, &r);

    // 2) Fill left and right side strips
    r = {
        float(rect.x),
        float(rect.y + radius),
        float(radius),
        float(rect.h - 2*radius)
    };
    SDL_RenderFillRect(Sys::renderer, &r);

    r = {
        float(rect.x + rect.w - radius),
        float(rect.y + radius),
        float(radius),
        float(rect.h - 2*radius)
    };
    SDL_RenderFillRect(Sys::renderer, &r);

    // Helper to build a corner fan
    auto drawCorner = [&](float cx, float cy, float startDeg){
        // Build triangle‐fan vertices
        std::vector<SDL_Vertex> verts;
        verts.reserve(segments + 2);

        // Center vertex
        verts.push_back({ {cx, cy}, fcol, {0,0} });

        // Arc points
        for (int i = 0; i <= segments; ++i) {
            float ang = (startDeg + (90.0f * i / segments)) * (float(M_PI)/180.0f);
            verts.push_back({
                { cx + radius * std::cos(ang),
                  cy + radius * std::sin(ang) },
                fcol,
                {0,0}
            });
        }

        // Build indices for the fan
        std::vector<int> idx;
        idx.reserve(segments * 3);
        for (int i = 1; i <= segments; ++i) {
            idx.push_back(0);
            idx.push_back(i);
            idx.push_back(i+1);
        }

        // Draw
        SDL_RenderGeometry(Sys::renderer,
                           /*texture=*/ nullptr,
                           verts.data(), (int)verts.size(),
                           idx.data(),   (int)idx.size());
    };

    // 3) Draw four corners
    drawCorner(rect.x + radius,               rect.y + radius,               180.0f); // TL
    drawCorner(rect.x + rect.w - radius,     rect.y + radius,               270.0f); // TR
    drawCorner(rect.x + rect.w - radius,     rect.y + rect.h - radius,       0.0f); // BR
    drawCorner(rect.x + radius,               rect.y + rect.h - radius,      90.0f); // BL
}






// A helper function that always produces (segments+1) vertices per corner,
// even if the radius is 0 (in which case it outputs duplicate vertices).
std::vector<SDL_Vertex> computeRoundedRectPolygonFixed(
    const SDL_Rect& rect,
    const BorderRadiusRect& br,
    const SDL_Color& color,
    int segments = 8)
{
    std::vector<SDL_Vertex> vertices;
    vertices.reserve((segments+1)*4);

    auto pushV = [&](float x, float y){
        SDL_Vertex v;
        v.position  = {x,y};
        v.color     = TO_FCOLOR(color);
        v.tex_coord = {0,0};
        vertices.push_back(v);
    };

    auto makeArc = [&](float cx, float cy, float startA, float endA, unsigned r){
        std::vector<SDL_FPoint> pts;
        pts.reserve(segments+1);
        for (int i=0; i<=segments; ++i){
            float t = float(i)/segments;
            float a = (startA + t*(endA-startA)) * (M_PI/180.f);
            pts.push_back({ cx + r*cos(a), cy + r*sin(a) });
        }
        return pts;
    };

    // Build each of the four corner arcs
    auto arcTL = makeArc(rect.x + br.top_left,     rect.y + br.top_left,     180, 270, br.top_left);
    auto arcTR = makeArc(rect.x + rect.w - br.top_right, rect.y + br.top_right,    270, 360, br.top_right);
    auto arcBR = makeArc(rect.x + rect.w - br.bottom_right, rect.y + rect.h - br.bottom_right, 0, 90,  br.bottom_right);
    auto arcBL = makeArc(rect.x + br.bottom_left,  rect.y + rect.h - br.bottom_left,  90, 180, br.bottom_left);

    // 1) Push *all* of arcTL
    for (auto &p : arcTL) pushV(p.x, p.y);
    // 2) Push arcTR *except* its first point (already at end of arcTL)
    for (int i=1; i<(int)arcTR.size(); ++i) pushV(arcTR[i].x, arcTR[i].y);
    // 3) arcBR, skip its first
    for (int i=1; i<(int)arcBR.size(); ++i) pushV(arcBR[i].x, arcBR[i].y);
    // 4) arcBL, skip its first
    for (int i=1; i<(int)arcBL.size(); ++i) pushV(arcBL[i].x, arcBL[i].y);

    return vertices;
}



// Helper: Linear interpolation along a polyline.
// poly: vector of SDL_FPoint representing a closed polyline.
// cumLengths: cumulative distances along poly (same size as poly).
// d: distance along the polyline at which to interpolate.
SDL_FPoint interpolatePoint(
    const std::vector<SDL_FPoint>& poly, 
    const std::vector<float>& cumLengths, 
    float d
) {
    size_t i = 0;
    while(i < cumLengths.size()-1 && cumLengths[i+1] < d)
        i++;
    float segLength = cumLengths[i+1] - cumLengths[i];
    float t = (segLength > 0.f) ? (d - cumLengths[i]) / segLength : 0.f;
    SDL_FPoint p;
    p.x = poly[i].x + t * (poly[i+1].x - poly[i].x);
    p.y = poly[i].y + t * (poly[i+1].y - poly[i].y);
    return p;
}



// Helper: Draw a dashed line along a straight edge.
// Given the start and end points, dashSize, and dashGapSize, this function computes
// the best dash count so that the side starts and ends with a full dash.
// It then draws each dash segment with the provided thickness.
void drawDashedLineRect(
    const SDL_Point& start, const SDL_Point& end,
    const SDL_Color& color, int thickness,
    int desiredDashSize, int desiredGapSize
) {
    // Compute the vector and length of the line.
    float dx = static_cast<float>(end.x - start.x);
    float dy = static_cast<float>(end.y - start.y);
    float L = std::sqrt(dx * dx + dy * dy);
    if(L < 1e-6f) return;

    // We need at least 2 dashes to have a gap.
    int bestN = 0;
    float bestGap = 0.f;
    float bestDiff = 1e9;

    // Try dash counts from 2 to a maximum (limit by L/dashSize + 1)
    int maxN = static_cast<int>(L / desiredDashSize) + 1;
    for (int N = 2; N <= maxN; N++) {
        // Total length covered by dashes: N * dashSize.
        // The number of gaps is (N - 1) and the gap size required to fill L:
        float candidateGap = (L - N * desiredDashSize) / (N - 1);
        if (candidateGap < 0) continue; // Not possible if dashes exceed L.

        float diff = std::abs(candidateGap - desiredGapSize);
        if(diff < bestDiff) {
            bestDiff = diff;
            bestN = N;
            bestGap = candidateGap;
        }
    }
    // If no valid segmentation is found, fallback to a single dash (solid line)
    if(bestN < 2) {
        drawThickLineSegment(start, end, color, thickness);
        return;
    }

    // Compute unit vector for the line.
    float ux = dx / L;
    float uy = dy / L;

    float pattern = desiredDashSize + bestGap;
    // Draw bestN dashes along the line.
    for (int i = 0; i < bestN; i++) {
        float dashStart = i * pattern;
        float dashEnd = dashStart + desiredDashSize;
        // Ensure the last dash doesn't exceed the line length.
        dashEnd = std::min(dashEnd, L);
        SDL_Point segStart = {
            start.x + static_cast<int>(ux * dashStart),
            start.y + static_cast<int>(uy * dashStart)
        };
        SDL_Point segEnd = {
            start.x + static_cast<int>(ux * dashEnd),
            start.y + static_cast<int>(uy * dashEnd)
        };
        drawThickLineSegment(segStart, segEnd, color, thickness);
    }
}



void drawRoundedOutlineRect(
    const SDL_Rect& dRect,
    const int thickness,
    const BorderRadiusRect& borderRadius,
    const SDL_Color& color, 
    const int arcSegments
){
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

    SDL_RenderGeometry(
        Sys::renderer, 
        nullptr,
        borderVerts.data(), 
        static_cast<int>(borderVerts.size()),
        indices.data(), 
        static_cast<int>(indices.size())
    );
}



std::vector<SDL_FPoint> computeCornerArc(
    float cx, float cy, 
    float startAngle, float endAngle, 
    unsigned int r, int segments = 8
) {
    std::vector<SDL_FPoint> pts;
    for (int i = 0; i <= segments; i++) {
        float t = static_cast<float>(i) / segments;
        float angle = (startAngle + t * (endAngle - startAngle)) * (M_PI / 180.f);
        pts.push_back({ cx + r * cos(angle), cy + r * sin(angle) });
    }
    return pts;
}



// Helper to draw a dashed line along a straight edge.
void drawDashedEdge(
    const SDL_Point& start, const SDL_Point& end,
    const SDL_Color& color, int thickness,
    int dashSize, int gapSize
) {
    float dx = static_cast<float>(end.x - start.x);
    float dy = static_cast<float>(end.y - start.y);
    float L = std::sqrt(dx * dx + dy * dy);
    if (L < 1e-6f) return;

    float ux = dx / L;
    float uy = dy / L;

    float pattern = dashSize + gapSize;
    int n = static_cast<int>(std::floor(L / pattern));
    if(n < 1) n = 1;

    // Adjust dash length so that the pattern fits exactly:
    float remainder = L - n * pattern;
    float adjustedGap = gapSize + remainder / n;
    float effectivePattern = dashSize + adjustedGap;

    float offset = 0.f;
    for (int i = 0; i < n; i++) {
        float dashStart = offset;
        float dashEnd = offset + dashSize;

        if(dashEnd > L) dashEnd = L;

        SDL_Point p1 = { start.x + static_cast<int>(ux * dashStart),
                start.y + static_cast<int>(uy * dashStart) };
        SDL_Point p2 = { start.x + static_cast<int>(ux * dashEnd),
                start.y + static_cast<int>(uy * dashEnd) };
        
        drawThickLineSegment(p1, p2, color, thickness);
        offset += effectivePattern;
    }
}




void drawRoundedCorners(const SDL_Rect& dRect, const BorderRadiusRect& br, const SDL_Color& color, int thickness, int arcSegments = 8) {
    // Top-left corner
    {
        float cx = dRect.x + br.top_left;
        float cy = dRect.y + br.top_left;
        std::vector<SDL_FPoint> arc = computeCornerArc(cx, cy, 180.f, 270.f, br.top_left, arcSegments);
        for (size_t i = 0; i < arc.size()-1; i++) {
            SDL_Point p1 = { static_cast<int>(arc[i].x), static_cast<int>(arc[i].y) };
            SDL_Point p2 = { static_cast<int>(arc[i+1].x), static_cast<int>(arc[i+1].y) };
            drawThickLineSegment(p1, p2, color, thickness);
        }
    }
    // Top-right corner
    {
        float cx = dRect.x + dRect.w - br.top_right;
        float cy = dRect.y + br.top_right;
        std::vector<SDL_FPoint> arc = computeCornerArc(cx, cy, 270.f, 360.f, br.top_right, arcSegments);
        for (size_t i = 0; i < arc.size()-1; i++) {
            SDL_Point p1 = { static_cast<int>(arc[i].x), static_cast<int>(arc[i].y) };
            SDL_Point p2 = { static_cast<int>(arc[i+1].x), static_cast<int>(arc[i+1].y) };
            drawThickLineSegment(p1, p2, color, thickness);
        }
    }
    // Bottom-right corner
    {
        float cx = dRect.x + dRect.w - br.bottom_right;
        float cy = dRect.y + dRect.h - br.bottom_right;
        std::vector<SDL_FPoint> arc = computeCornerArc(cx, cy, 0.f, 90.f, br.bottom_right, arcSegments);
        for (size_t i = 0; i < arc.size()-1; i++) {
            SDL_Point p1 = { static_cast<int>(arc[i].x), static_cast<int>(arc[i].y) };
            SDL_Point p2 = { static_cast<int>(arc[i+1].x), static_cast<int>(arc[i+1].y) };
            drawThickLineSegment(p1, p2, color, thickness);
        }
    }
    // Bottom-left corner
    {
        float cx = dRect.x + br.bottom_left;
        float cy = dRect.y + dRect.h - br.bottom_left;
        std::vector<SDL_FPoint> arc = computeCornerArc(cx, cy, 90.f, 180.f, br.bottom_left, arcSegments);
        for (size_t i = 0; i < arc.size()-1; i++) {
            SDL_Point p1 = { static_cast<int>(arc[i].x), static_cast<int>(arc[i].y) };
            SDL_Point p2 = { static_cast<int>(arc[i+1].x), static_cast<int>(arc[i+1].y) };
            drawThickLineSegment(p1, p2, color, thickness);
        }
    }
}









void GUI::renderRect(
    const SDL_Rect& dRect,
    const SDL_Color& color,
    const int thickness,
    const BorderRadiusRect& borderRadius
){
    // ----------------------------------------------------------------------------------------
    // UNDASHED RECT ----------------------------------------------------------------------------   
    
    int arcSegments = 8;

    // Options: 
    // RectFill    Corners      Style
    //  Filled      Regular      Solid
    //  Filled      Rounded      Solid

    //  UnFilled    Regular      Solid
    //  UnFilled    Regular      Dashed

    //  UnFilled    Rounded      Solid
    //  UnFilled    Rounded      Dashed



    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    // FIRST WE DEAL WITH RECTS THAT ARE FILLED, SOLID
    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    
    // If no rounded corners are requested, use SDL’s routines.
    bool noRounded = (borderRadius.top_left == 0 && borderRadius.top_right == 0 &&
        borderRadius.bottom_left == 0 && borderRadius.bottom_right == 0);


    if(thickness == -1){
        SDL_FRect fRect = TO_FRECT(dRect);
        if(noRounded) {
            // FILLED [NOT ROUNDED] RECT
            SDL_SetRenderDrawColor(Sys::renderer, color);
            SDL_RenderFillRect(Sys::renderer, &fRect);
        }
        else{
            // FULL [ROUNDED] RECT
            drawRoundedFilledRect(dRect, borderRadius.top_left, color);
        }

        return;
    }



    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    // NOW WE DEAL WITH RECTS THAT ARE OUTLINES, SOLID LINES, NO DASHES
    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------

    if(!pDashLine) {
        // The function will detect if the borderRadius is 0, so it wont round the corners
        drawRoundedOutlineRect(dRect, thickness, borderRadius, color, arcSegments);

        return;
    }
    


    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    // NOW WE DEAL WITH RECTS THAT ARE DASHED, NON-ROUNDED AND THEN ROUNEDED ----------------------
    // --------------------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------------------
    pDashLine = false; // Reset the pushed style

    if (noRounded) {
        SDL_Point topLeft     = { dRect.x, dRect.y };
        SDL_Point topRight    = { dRect.x + dRect.w, dRect.y };
        SDL_Point bottomRight = { dRect.x + dRect.w, dRect.y + dRect.h };
        SDL_Point bottomLeft  = { dRect.x, dRect.y + dRect.h };
        
        // For each edge, adjust gap size so that the dash pattern fits exactly.
        drawDashedLineRect(topLeft, topRight, color, thickness, pDashSize, pDashGapSize);
        drawDashedLineRect(topRight, bottomRight, color, thickness, pDashSize, pDashGapSize);
        drawDashedLineRect(bottomRight, bottomLeft, color, thickness, pDashSize, pDashGapSize);
        drawDashedLineRect(bottomLeft, topLeft, color, thickness, pDashSize, pDashGapSize);

        // Fill the corners with small solid squares to cover any gaps.
        SDL_FRect cornerRect;
        // Adjust the size of the filler based on thickness.
        int fillerSize = thickness; // or adjust as needed

        int offset = 0;
        if(thickness % 2) offset = 1;

        // Top-left corner filler.
        cornerRect = { 
            static_cast<float>(dRect.x - fillerSize/2 - offset), 
            static_cast<float>(dRect.y - fillerSize/2 - offset), 
            static_cast<float>(fillerSize), 
            static_cast<float>(fillerSize) 
        };
        SDL_RenderFillRect(Sys::renderer, &cornerRect);

        // Top-right corner filler.
        cornerRect = { 
            static_cast<float>(dRect.x + dRect.w - fillerSize/2 - offset), 
            static_cast<float>(dRect.y - fillerSize/2 - offset),
            static_cast<float>(fillerSize), 
            static_cast<float>(fillerSize) 
        };
        SDL_RenderFillRect(Sys::renderer, &cornerRect);

        // Bottom-right corner filler.
        cornerRect = { 
            static_cast<float>(dRect.x + dRect.w - fillerSize/2 - offset), 
            static_cast<float>(dRect.y + dRect.h - fillerSize/2 - offset),
            static_cast<float>(fillerSize), 
            static_cast<float>(fillerSize) 
        };
        SDL_RenderFillRect(Sys::renderer, &cornerRect);

        // Bottom-left corner filler.
        cornerRect = { 
            static_cast<float>(dRect.x - fillerSize/2 - offset), 
            static_cast<float>(dRect.y + dRect.h - fillerSize/2 - offset), 
            static_cast<float>(fillerSize), 
            static_cast<float>(fillerSize) 
        };
        SDL_RenderFillRect(Sys::renderer, &cornerRect);
        return;
    }





    // First, draw the solid curved corners.
    drawRoundedCorners(dRect, borderRadius, color, thickness, arcSegments);

    // Determine endpoints of each arc:
    // For each corner, we compute the arc endpoints using our computeCornerArc.
    // Top-left arc endpoints:
    std::vector<SDL_FPoint> arcTL = computeCornerArc(dRect.x + borderRadius.top_left, dRect.y + borderRadius.top_left, 180.f, 270.f, borderRadius.top_left, arcSegments);
    std::vector<SDL_FPoint> arcTR = computeCornerArc(dRect.x + dRect.w - borderRadius.top_right, dRect.y + borderRadius.top_right, 270.f, 360.f, borderRadius.top_right, arcSegments);
    std::vector<SDL_FPoint> arcBR = computeCornerArc(dRect.x + dRect.w - borderRadius.bottom_right, dRect.y + dRect.h - borderRadius.bottom_right, 0.f, 90.f, borderRadius.bottom_right, arcSegments);
    std::vector<SDL_FPoint> arcBL = computeCornerArc(dRect.x + borderRadius.bottom_left, dRect.y + dRect.h - borderRadius.bottom_left, 90.f, 180.f, borderRadius.bottom_left, arcSegments);

    // The dash edges are drawn from:
    // Top edge: from arcTL's last point to arcTR's first point.
    SDL_Point topEdgeStart = { static_cast<int>(arcTL.back().x), static_cast<int>(arcTL.back().y) };
    SDL_Point topEdgeEnd   = { static_cast<int>(arcTR.front().x), static_cast<int>(arcTR.front().y) };

    // Right edge: from arcTR's last point to arcBR's first point.
    SDL_Point rightEdgeStart = { static_cast<int>(arcTR.back().x), static_cast<int>(arcTR.back().y) };
    SDL_Point rightEdgeEnd   = { static_cast<int>(arcBR.front().x), static_cast<int>(arcBR.front().y) };

    // Bottom edge: from arcBR's last point to arcBL's first point.
    SDL_Point bottomEdgeStart = { static_cast<int>(arcBR.back().x), static_cast<int>(arcBR.back().y) };
    SDL_Point bottomEdgeEnd   = { static_cast<int>(arcBL.front().x), static_cast<int>(arcBL.front().y) };

    // Left edge: from arcBL's last point to arcTL's first point.
    SDL_Point leftEdgeStart = { static_cast<int>(arcBL.back().x), static_cast<int>(arcBL.back().y) };
    SDL_Point leftEdgeEnd   = { static_cast<int>(arcTL.front().x), static_cast<int>(arcTL.front().y) };

    // Draw dashed lines on each straight edge.
    drawDashedEdge(topEdgeStart, topEdgeEnd, color, thickness, pDashSize, pDashGapSize);
    drawDashedEdge(rightEdgeStart, rightEdgeEnd, color, thickness, pDashSize, pDashGapSize);
    drawDashedEdge(bottomEdgeStart, bottomEdgeEnd, color, thickness, pDashSize, pDashGapSize);
    drawDashedEdge(leftEdgeStart, leftEdgeEnd, color, thickness, pDashSize, pDashGapSize);
}








void GUI::Rect(
    const SDL_Rect& dRect, 
    const SDL_Color& color, 
    const int thickness
) {
    if(dRect.w < 1 && dRect.h < 1){
        cout << "Invalid Rect size" << endl;
        return;
    }
    
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
    
    // ---------------------------------------------------------------------------------------

    TextureData canvas;
    SDL_Texture* tex = SDL_CreateTexture(
        Sys::renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        dRect.w,
        dRect.h
    );
    if(tex == nullptr) {
        Sys::printf_err(TM_TEXTURE_CREATE_ERROR);
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "SDL_CreateTexture error: %s\n", SDL_GetError());
        return;
    }

    // in GUI::Rect, after creating 'tex':
    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_LINEAR);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

    canvas.setTexture(tex);
    canvas.reloadInfo();

    SDL_Rect mutable_dRect = dRect;
    mutable_dRect.x = 0;
    mutable_dRect.y = 0;

    auto oldRenderTarget = SDL_GetRenderTarget(Sys::renderer);
    SDL_SetRenderTarget(Sys::renderer, canvas.getTexture());
    
    SDL_SetRenderDrawColor(Sys::renderer, 0, 0, 0, 0);
    SDL_RenderClear(Sys::renderer);
    renderRect(mutable_dRect, color, thickness, borderRadius);
    
    SDL_SetRenderTarget(Sys::renderer, oldRenderTarget);

    mutable_dRect = dRect;
    GUI::Image(canvas, mutable_dRect);

    SDL_DestroyTexture(canvas.getTexture());
    canvas.setTexture(nullptr);

    return;
}