project('vsFilterScript', 'cpp', 
    default_options: ['buildtype=release', 'b_ndebug=if-release', 'cpp_std=c++2a'],
    version : '2'
)

if host_machine.cpu_family().startswith('x86')
  add_project_arguments('-mfpmath=sse', '-msse2', language : 'cpp')
endif

pkg = import('pkgconfig')
deps = dependency('vapoursynth')

incdir = include_directories('include')

test_plug = ['Examples/EntryPoint.cxx']

headers = [
    'include/AudioFrame.vxx',
    'include/AudioNode.vxx',
    'include/Core.vxx',
    'include/Frame.vxx',
    'include/Function.vxx',
    'include/Globals.vxx',
    'include/Infrastructure.vxx',
    'include/Interface.vxx',
    'include/Map.vxx',
    'include/Metadata.vxx',
    'include/Node.vxx',
    'include/Plane.vxx',
    'include/Plugin.vxx',
    'include/Range.vxx',
    'include/ResourceManager.vxx',
    'include/Utility.vxx',
    'include/VideoFrame.vxx',
    'include/VideoNode.vxx',
]

examples = [
    'Examples/Crop.hxx',
    'Examples/EntryPoint.cxx',
    'Examples/GaussBlurFast.hxx',
    'Examples/GaussBlur.hxx',
    'Examples/ModifyFrameExample.vpy',
    'Examples/ModifyFrame.hxx',
    'Examples/nnedi3_rpow2.hxx',
    'Examples/Palette.hxx',
    'Examples/Rec601ToRGB.hxx',
    'Examples/TemporalMedian.hxx',
]

if get_option('install_examples')
    install_data(examples, install_dir : 'share/vsFilterScript/Examples')
endif

if get_option('build_examples')
    shared_module('vsfilterscriptexample',
        test_plug,
        include_directories : incdir,
        dependencies : deps,
        gnu_symbol_visibility : 'hidden',
        install_dir : join_paths(deps.get_pkgconfig_variable('libdir'), 'vapoursynth'),
        install : true
    )
endif

install_headers(headers, subdir : 'vsFilterScript')

pkg.generate(
    name : 'vsfilterscript',
    subdirs : 'vsFilterScript',
    version : meson.project_version(),
    description : 'C++ wrapper for VSAPI to help you sketch your filter in the fastest possible way.'
)
