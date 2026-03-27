import json
import optparse
import sys

def _ParseOptions():
    parser = optparse.OptionParser()
    parser.add_option('-i',
                      '--input_file',
                      help='input version json5')
    parser.add_option('-o',
                      '--output_file',
                      help='output version file')
    options, _ = parser.parse_args()
    return options

def ReadHeader(options):
    arkwebVersion = "4.1.6.1"
    arkwebVersionCode = 410061000
    arkwebVersionFull = "4.1.6.1"
    with open(options.input_file, 'r') as f:
        data = json.load(f)
        if data is None:
            return [arkwebVersion, arkwebVersionCode, arkwebVersionFull]
        if "app" in data and "versionName" in data["app"]:
            arkwebVersion = data["app"]["versionName"].split("sp")[0]
            arkwebVersionCode = data["app"]["versionCode"]
            arkwebVersionFull = versionName
    return [arkwebVersion, arkwebVersionCode, arkwebVersionFull]

def WriteHeader(options, version):
    header_macros = "ARKWEB_VERSION_H"
    with open(options.output_file, "w") as output_file:
        output_file.write('#ifndef %s\n' % header_macros)
        output_file.write('#define %s\n\n' % header_macros)
        output_file.write('\n#define ARKWEB_VERSION \"%s\"\n' % version[0])
        output_file.write('\n#define ARKWEB_VERSION_CODE %s\n' % version[1])
        output_file.write('\n#define ARKWEB_VERSION_FULL \"%s"\n' % version[2])
        output_file.write('\n#endif  // %s\n' % header_macros)


if '__main__' == __name__:
    options = _ParseOptions()
    WriteHeader(options, ReadHeader(options))
