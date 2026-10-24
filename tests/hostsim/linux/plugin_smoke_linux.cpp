/* SPDX-License-Identifier: MIT */
/* Linux form of plugin_smoke.cpp: the plugin's own Turnip sequence (drivers/NativeMetal/NativeTurnipSmoke.h: fill, compute pipeline, render
 * pass) run directly, without the Mach watchdog, against the host copy of the kernel stack. The software CP executes nothing, so the expected
 * outcome is the same as on the Mac: no setup failure, the untouched half of the fill buffer (512 words), no compute writes, and the render
 * read-back keeps its 0x5a pattern (4096 stray pixels). Both pipelines are compiled to IR3 for the GPU the kernel reports. */
#include "NativeTurnipSmoke.h"

int main(int argc, char **argv) {
    if (argc < 2) return 2;
    unsigned marks[32], count = 0;
    auto marker = [&](uint32_t value) { if (count < 32) marks[count++] = value; };
    const turnip::Result r = turnip::run(marker, argv[1]);
    printf("markers:");
    for (unsigned i = 0; i < count; ++i) printf(" %x", marks[i]);
    printf("\nstage %d result %d fill %d compute %d (step %d) render %d/%d (step %d) ok %d msg '%s'\n", r.stage, r.vkResult, r.verified,
           r.computeWords, r.computeStage, r.renderTriangle, r.renderOther, r.renderStage, r.ok, turnip::gFirstError);
    return r.stage == 0 && r.verified == 512 && r.computeWords == 0 && r.computeStage == 0 && r.renderStage == 0 && r.renderTriangle == 0 &&
           r.renderOther == 4096 ? 0 : 1;
}
