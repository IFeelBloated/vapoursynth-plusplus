import vapoursynth as vs
core = vs.get_core()
clp = core.colorbars.ColorBars(0, vs.YUV444P12)
clp = core.fmtc.bitdepth(clp, bits=32, fulls=False, fulld=True)
clp = core.fmtc.matrix(clp, "601", fulls=True, fulld=True)
clp *= 10000

def GaussBlur(src):
    dst = src.copy()
    for c in range(dst.format.num_planes):
        plane = dst.get_write_array(c)
        for y in range(1, len(plane)-1):
            for x in range(1, len(plane[0])-1):
                plane[y][x] = (plane[y-1][x-1] + plane[y-1][x]*2 + plane[y-1][x+1] + \
                              plane[y][x-1]*2 + plane[y][x]*4 + plane[y][x+1]*2 + \
                              plane[y+1][x-1] + plane[y+1][x]*2 + plane[y+1][x+1]) / 16
    return dst

clp = core.test.ModifyFrame(clp, GaussBlur)
clp.set_output()