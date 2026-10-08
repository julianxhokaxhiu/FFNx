# Image-based lighting (IBL)

With `enable_lighting = true`, FF7's advanced lighting can light models with an environment: reflections and ambient
light taken from cubemaps instead of a flat ambient colour. Reflective materials (metal, glossy gltf models) look dark
without one.

FFNx looks for the cubemaps under `<external_lighting_path>/ibl/` (`lighting/ibl/` by default), in DDS format:

| File | What it is |
| --- | --- |
| `field_<id>_s.dds` + `field_<id>_d.dds` | A field's pair (`<id>` is the field id, see FFNx.log with `trace_loaders`) |
| `bat_<id>_s.dds` + `bat_<id>_d.dds` | A battle's pair (`<id>` is the battle id) |
| `world_s.dds` + `world_d.dds` | The world map's pair |
| `default_s.dds` + `default_d.dds` | Used by every scene that has no pair of its own |
| `envBrdf.dds` | The BRDF lookup table of the split-sum approximation (required once for all IBL) |

`_s` is the specular cubemap: a prefiltered radiance map whose mip levels get blurrier with roughness. The shader
picks the mip from the roughness as `lod = mipCount - 1 - (3 - 1.15 * log2(roughness))`, so a 128x128 cubemap with
8 mips is sampled at roughness 0.09 on mip 0, 0.3 on mip 2, 1.0 on mip 4 and above.
`_d` is the diffuse irradiance cubemap (the cosine-weighted average radiance around each direction; a small one such
as 32x32 is enough). Both are read in linear colour; a floating-point format (RGBA16F) keeps bright skies and suns.
`envBrdf.dds` is a 2D lookup where x is `N.V` and y is `1 - roughness`, holding the scale in red and the bias in green.
+Y is up in the cubemaps.

Tools such as cmftStudio or IBLBaker produce the two cubemaps from an HDR panorama; a generated default set comes
with the mod that provides the models. Set `trace_loaders = true` to see which pair a scene uses in FFNx.log
("Lighting: field_154 has no IBL cubemaps, using the default pair").
