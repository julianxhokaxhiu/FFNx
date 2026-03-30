# This vs script converts from the insane YUV format found in bink v1 videos created circa 1999 to RGB.
# (Further conversion from RGB back to rec709 YUV is shown in a comment.)

# Dependencies: fmtconv plugin

import vapoursynth as vs
core = vs.core

# Set your input file here.
video = core.bs.VideoSource('003.bik')
#video = core.bs.VideoSource('tespattern.bik')
#video = core.bs.VideoSource('red.bik')

# Convert to 32-bit float for doing math.
# Bink is inherently limited range.
# TV->PC range conversion occurs during conversion to float
# (For float32 output, fulld is always True whether you set it or not.)
video = core.fmtc.bitdepth(video, bits=32, fulls=False, fulld=True)
# Resample to 4:4:4 so the planes are the same size 
video = core.fmtc.resample(video,css='444', fulls=True, fulld=True)

# Split planes
Yin = core.std.ShufflePlanes(video, planes=0, colorfamily=vs.GRAY)
Uin = core.std.ShufflePlanes(video, planes=1, colorfamily=vs.GRAY)
Vin = core.std.ShufflePlanes(video, planes=2, colorfamily=vs.GRAY)

# Bink uses 16-234 instead of 16-235 for luma. Correct by multiplying by 219/218.
Yin = core.std.Expr(clips=[Yin], expr=["x 219 218 / *"])

# Matrix by the inverse of the bink encoder's observable RGB-to-YUV behavior.
# This is an incorrect rec601 matrix in which:
#   - E'r and E'b, the weights for red and blue's contributions to Y (0.299 and 0.114, respectively), are transposed.
#   - The scaling factors for U and V (1.772 and 1.402, respectively) are transposed.
Rout = core.std.Expr(clips=[Yin, Uin, Vin], expr=["x z 1.772 * + "])
Gout = core.std.Expr(clips=[Yin, Uin, Vin], expr=["x y -0.299 1.402 * 0.587 / * + z -0.114 1.772 * 0.587 / * + "])
Bout = core.std.Expr(clips=[Yin, Uin, Vin], expr=["x y 1.402 * +"])

# Clamp any out-of-range outputs to 0-1.
Rout = core.std.Expr(clips=[Rout], expr=["x 0 max 1 min"])
Gout = core.std.Expr(clips=[Gout], expr=["x 0 max 1 min"])
Bout = core.std.Expr(clips=[Bout], expr=["x 0 max 1 min"])

# Combine planes into a single clip and cast to RGB
RGBout = core.std.ShufflePlanes(clips=[Rout, Gout, Bout], planes=[0, 0, 0], colorfamily=vs.RGB)

# Set video to our RGB output clip
video = RGBout

###### Alternatively, we could convert to a sane YUV format for output or further processing
# Convert to rec709 YUV
# (Make sure to set the metadata in your output file!)
# video = core.fmtc.matrix(video, mat='709', fulls=True, fulld=True)
# Set color range clip property.
# (Make sure to set encoder parameters and the metadata in your output file!)
# video = core.std.SetFrameProp(video, prop='_ColorRange', intval=0) # 0 means full range
# Back to 4:2:0 for encoding
# video = core.fmtc.resample(video,css='420', fulls=True, fulld=True)
# Down to 10 bits for encoding
# video = core.fmtc.bitdepth(video, bits=10, fulls=True, fulld=True)
######

# Set output clip
video.set_output()
