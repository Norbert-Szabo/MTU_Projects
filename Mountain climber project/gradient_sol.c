#include <float.h>
#include "gradient.h"


/**
 * Checks for plateau, if true then no meaningful gradient can be found
 */
static int is_plateau(float min_val, float max_val) {
    return (max_val - min_val) < 0.5f;
}

/**
 * @brief Finds local  min and max values and corresponding coordinates
 *
 * @param view      Current local view
 * @param minX      Output parameter X coordinate of min value
 * @param minY      Output parameter Y coordinate of min value
 * @param maxX      Output parameter X coordinate of max value
 * @param maxY      Output parameter Y coordinate of max value
 * @param min_val   Output parameter for min value in view
 * @param max_val   Output parameter for max value in view
 */
static void find_min_max(float view[VIEW_SIZE][VIEW_SIZE],
                         int *minX, int *minY, int *maxX, int *maxY,
                         float *min_val, float *max_val) {
    *min_val = FLT_MAX; *max_val = -FLT_MAX;
    *minX = *minY = *maxX = *maxY = VIEW_RADIUS;
    for (int i = 0; i < VIEW_SIZE; i++) {
        for (int j = 0; j < VIEW_SIZE; j++) {
            if (view[i][j] == -1) continue; // skip out-of-bounds cells
            if (view[i][j] < *min_val) { *min_val = view[i][j]; *minX = j; *minY = i; }
            if (view[i][j] > *max_val) { *max_val = view[i][j]; *maxX = j; *maxY = i; }
        }
    }
}

/**
 * @brief If is_plateau() returns true, this function is called to jump to
 * away from the plateau
 *
 * @param view      current local view
 * @param curX      current position X coordinate
 * @param curY      current position Y coordinate
 * @param min_val   min value in view
 * @param max_val   max value in view
 * @param minX      min value X coordinate
 * @param minY      min value Y coordinate
 * @param maxX      max value X coordinate
 * @param maxY      max value Y coordinate
 *
 * @note minX, minY, maxX, maxY and max_val are only used to find new
 * highest point once plateau has been escaped
 */
static void escape_plateau(float view[VIEW_SIZE][VIEW_SIZE], int *curX, int *curY,
                            float *min_val, float *max_val,
                            int *minX, int *minY, int *maxX, int *maxY) {
    int jumpX = 0, jumpY = 0;

    // Look at the top/bottom edges of the view: if they're still "flat"
    // (close to min_val), assume the plateau continues that way.
    for (int j = 0; j < VIEW_SIZE; j++) {
        if (view[0][j] != -1 && view[0][j] < *min_val + 0.5f) jumpY = 1;
        if (view[VIEW_SIZE-1][j] != -1 && view[VIEW_SIZE-1][j] < *min_val + 0.5f) jumpY = -1;
    }
    // Same idea for the left/right edges, to decide jumpX.
    for (int i = 0; i < VIEW_SIZE; i++) {
        if (view[i][0] != -1 && view[i][0] < *min_val + 0.5f) jumpX = 1;
        if (view[i][VIEW_SIZE-1] != -1 && view[i][VIEW_SIZE-1] < *min_val + 0.5f) jumpX = -1;
    }
    // No clear edge signal: default to jumping diagonally.
    if (jumpX == 0 && jumpY == 0) { jumpX = 1; jumpY = 1; }

    // Jump a full view-width/height for next view to not overlap with previous view.
    *curX += jumpX * VIEW_SIZE;
    *curY += jumpY * VIEW_SIZE;
    if (*curX < VIEW_RADIUS) *curX = VIEW_RADIUS; // clamp against the map edge
    if (*curY < VIEW_RADIUS) *curY = VIEW_RADIUS;

    generate_view(view, *curY, *curX);

    // Check if the jump landed partially out of bound.
    int oob = 0;
    for (int i = 0; i < VIEW_SIZE; i++) {
        for (int j = 0; j < VIEW_SIZE; j++) {
            if (view[i][j] == -1) { oob = 1; break; }
        }
        if (oob) break;
    }
    // If out of bounds, step back one unit at a time until not out of bounds
    int pb = 0;
    while (oob && pb++ < 20) {
        *curX -= jumpX;
        *curY -= jumpY;
        if (*curX < VIEW_RADIUS) *curX = VIEW_RADIUS;
        if (*curY < VIEW_RADIUS) *curY = VIEW_RADIUS;
        generate_view(view, *curY, *curX);
        oob = 0;
        for (int i = 0; i < VIEW_SIZE; i++) {
            for (int j = 0; j < VIEW_SIZE; j++) {
                if (view[i][j] == -1) { oob = 1; break; }
            }
            if (oob) break;
        }
    }
    // Recompute min/max to calculate an accurate direction of travel
    find_min_max(view, minX, minY, maxX, maxY, min_val, max_val);
}

/**
 * Find the highest point (peak) by traveling along the steepest gradient
 */
path_point find_highest_point() {
    float view[VIEW_SIZE][VIEW_SIZE];
    float min_val, max_val;
    int minX, minY, maxX, maxY;

    // Start roughly in the middle of the landscape (at least 100x100, up to ~200x200)
    int curX = 100, curY = 100;
    int stepSize = 50;

    generate_view(view, curY, curX);
    find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);


    if (is_plateau(min_val, max_val))
        escape_plateau(view, &curX, &curY, &min_val, &max_val, &minX, &minY, &maxX, &maxY);

    // The "gradient":A vector pointing from min to max direction
    float dx = maxX - minX;
    float dy = maxY - minY;
    float lastDx = dx, lastDy = dy;      // gradient direction from previous step (to detect reversal)
    int lastX = curX, lastY = curY;      // position before the most recent step (to backtrack if needed)

    int steps = 0;
    int prevPrevX = -1, prevPrevY = -1;  // position two steps ago (to detect A->B->A oscillation)

    // gradient ascent
    while (steps++ < 80) {

        // if step size is less than view radius, jump to max point until peak is found
        if (stepSize < 5) {
            curX = curX + (maxX - VIEW_RADIUS); // offset of highest cell from view center
            curY = curY + (maxY - VIEW_RADIUS);
            if (curX < VIEW_RADIUS) curX = VIEW_RADIUS;
            if (curY < VIEW_RADIUS) curY = VIEW_RADIUS;
            generate_view(view, curY, curX);
            find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);
            break;
        }

        // Normalize the gradient vector (dx, dy) to a unit direction, then jump stepSize along it
        float len = sqrt(dx*dx + dy*dy);
        if (len < 0.0001f) break; // no gradient at all (shouldn't normally happen here)

        curX += (int)round((dx / len) * stepSize);
        curY += (int)round((dy / len) * stepSize);
        if (curX < VIEW_RADIUS) curX = VIEW_RADIUS;
        if (curY < VIEW_RADIUS) curY = VIEW_RADIUS;

        generate_view(view, curY, curX);

        // Check whether the new view has run off the edge of the map
        int oob = 0;
        for (int i = 0; i < VIEW_SIZE; i++) {
            for (int j = 0; j < VIEW_SIZE; j++) {
                if (view[i][j] == -1) { oob = 1; break; }
            }
            if (oob) break;
        }
        if (oob) {
            // Overstepped off the map: retreat to the last good position and halve the step size
            curX = lastX;
            curY = lastY;
            stepSize /= 2;
            generate_view(view, curY, curX);
            find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);
            dx = maxX - minX;
            dy = maxY - minY;
            continue;
        }

        find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);


        if (is_plateau(min_val, max_val))
            escape_plateau(view, &curX, &curY, &min_val, &max_val, &minX, &minY, &maxX, &maxY);

        // Recompute this view's local gradient direction for the next step
        dx = maxX - minX;
        dy = maxY - minY;

        // If oscillation  is detected, get midpoint and skip to fine tune
        if (curX == prevPrevX && curY == prevPrevY) {

            int midX = (curX + lastX) / 2;
            int midY = (curY + lastY) / 2;
            if (midX < VIEW_RADIUS) midX = VIEW_RADIUS;
            if (midY < VIEW_RADIUS) midY = VIEW_RADIUS;
            curX = midX; curY = midY;
            generate_view(view, curY, curX);
            find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);
            break;
        }

        // if "uphill" direction is reversed, go to previous location and half stepsize
        if (lastDx * dx + lastDy * dy < 0) {
            curX = lastX;
            curY = lastY;
            stepSize /= 2;
            generate_view(view, curY, curX);
            find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);
            dx = maxX - minX;
            dy = maxY - minY;
            continue;
        }

        // No reversal or oscillation detected: accept this step and record history
        prevPrevX = lastX; prevPrevY = lastY;
        lastDx = dx; lastDy = dy;
        lastX = curX; lastY = curY;
    }

    // Fine tune jumping to the highest local point until it is center of the view
    int truePeakX = curX + (maxX - VIEW_RADIUS); // absolute map coords of best guess so far
    int truePeakY = curY + (maxY - VIEW_RADIUS);
    float bestMax = max_val;
    int lastMoveX = 0, lastMoveY = 0; // direction of the most recent step
    for (int ft = 0; ft < 150; ft++) {
        if (maxX == VIEW_RADIUS && maxY == VIEW_RADIUS) break; // already centered on the highest point

        int nx = curX + (maxX - VIEW_RADIUS);
        int ny = curY + (maxY - VIEW_RADIUS);

        int trueNx = nx, trueNy = ny; // unclamped coords, used to detect "stuck at map edge"
        if (nx < VIEW_RADIUS) nx = VIEW_RADIUS;
        if (ny < VIEW_RADIUS) ny = VIEW_RADIUS;

        // If it stopped moving without being on the peak then break
        if (nx == curX && ny == curY && trueNx == truePeakX && trueNy == truePeakY) break;
        lastMoveX = nx - curX; lastMoveY = ny - curY;
        curX = nx; curY = ny;
        truePeakX = trueNx; truePeakY = trueNy;

        generate_view(view, curY, curX);
        find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);

        if (max_val < bestMax) { break; }
        bestMax = max_val;

        truePeakX = curX + (maxX - VIEW_RADIUS);
        truePeakY = curY + (maxY - VIEW_RADIUS);

        if (is_plateau(min_val, max_val))
            escape_plateau(view, &curX, &curY, &min_val, &max_val, &minX, &minY, &maxX, &maxY);
    }

    path_point peak;
    peak.x = truePeakX;
    peak.y = truePeakY;

    // if peak isn't found, exhaustive grid search is used to find it
    if (!declare_peak(peak.x, peak.y)) {

        int dirBestX = curX, dirBestY = curY;
        float dirBestMax = max_val;
        for (int step = 0; step < 2; step++) {
            curX += lastMoveX;
            curY += lastMoveY;
            if (curX < VIEW_RADIUS) curX = VIEW_RADIUS;
            if (curY < VIEW_RADIUS) curY = VIEW_RADIUS;
            generate_view(view, curY, curX);
            find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);
            if (max_val <= dirBestMax) break; // no improvement, stop probing this direction
            dirBestMax = max_val;
            dirBestX = curX; dirBestY = curY;
        }

        if (dirBestMax > max_val) {
            curX = dirBestX; curY = dirBestY;
            generate_view(view, curY, curX);
            find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);
        }

        // If the direction probe found higher ground, skip the exhaustive grid search
        if (dirBestMax > bestMax) {
            bestMax = dirBestMax;
        } else {
        // exhaustive local grid search
        int searchRadius = VIEW_SIZE;
        int bestSX = curX + (maxX - VIEW_RADIUS);
        int bestSY = curY + (maxY - VIEW_RADIUS);
        float bestSMax = max_val;
        for (int dy = -searchRadius; dy <= searchRadius; dy += VIEW_RADIUS) {
            for (int dx = -searchRadius; dx <= searchRadius; dx += VIEW_RADIUS) {
                int nx = curX + dx, ny = curY + dy;
                if (nx < VIEW_RADIUS) nx = VIEW_RADIUS;
                if (ny < VIEW_RADIUS) ny = VIEW_RADIUS;
                generate_view(view, ny, nx);
                find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);
                if (max_val > bestSMax) {
                    bestSMax = max_val;
                    bestSX = nx + (maxX - VIEW_RADIUS);
                    bestSY = ny + (maxY - VIEW_RADIUS);
                }
            }
        }

        // Move to the best point found by the grid probe
        curX = bestSX < VIEW_RADIUS ? VIEW_RADIUS : bestSX;
        curY = bestSY < VIEW_RADIUS ? VIEW_RADIUS : bestSY;
        generate_view(view, curY, curX);
        find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);
        bestMax = max_val;
        } // end of exhaustive grid search

        // Same fine tune from earlier from a new better position after grid search
        for (int ft = 0; ft < 150; ft++) {
            if (maxX == VIEW_RADIUS && maxY == VIEW_RADIUS) break;
            int nx = curX + (maxX - VIEW_RADIUS);
            int ny = curY + (maxY - VIEW_RADIUS);
            if (nx < VIEW_RADIUS) nx = VIEW_RADIUS;
            if (ny < VIEW_RADIUS) ny = VIEW_RADIUS;
            if (nx == curX && ny == curY) break;
            curX = nx; curY = ny;
            generate_view(view, curY, curX);
            find_min_max(view, &minX, &minY, &maxX, &maxY, &min_val, &max_val);
            if (max_val < bestMax) break;
            bestMax = max_val;
        }

        // Final answer for the second attempt, confirmed with declare_peak()
        peak.x = curX + (maxX - VIEW_RADIUS);
        peak.y = curY + (maxY - VIEW_RADIUS);
        declare_peak(peak.x, peak.y);
    }

    return peak;
}