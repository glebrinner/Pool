import json
import os
import re


with open("Release/Changelogs/version.txt") as changelog_file:
    BUILD_NUMBER = int(changelog_file.readline())

print("\033[1;36mStarting shipping build for mobile platforms (0.%d)\033[0m" % BUILD_NUMBER)

uproject_file = [f for f in os.listdir(os.getcwd()) if f.find(".uproject") != -1]

if len(uproject_file) != 1:
    print("  \033[0;31m.uproject file not found in working directory (%s)\033[0m" % os.getcwd())
    exit(1)

uproject_file = uproject_file[0]

print("  \033[0;32m%s found, patching PAD..\033[0m" % uproject_file)

with open(uproject_file) as json_file:
    data = json.load(json_file)
    to_delete = []
    for plugin_n in range(len(data["Plugins"])):
        if data["Plugins"][plugin_n]["Name"] == "GooglePAD":
            print("    \033[0;32mPAD found, disabling\033[0m")
            to_delete.append(plugin_n)
    deleted = 0
    for i in to_delete:
        del data["Plugins"][i-deleted]
        deleted += 1
    data["Plugins"].append({"Name": "GooglePAD", "Enabled": True})
with open(uproject_file, "w") as json_file:
    json.dump(data, json_file)

print("  \033[0;32mPAD patched\033[0m")

print("  \033[0;32mPatching configs..\033[0m")

with open ('Config/DefaultEngine.ini', 'r' ) as f:
    default_engine = f.read()
    default_engine = re.sub('r.MobileMSAA=.*$', r'r.MobileMSAA=4', default_engine, flags = re.M)
    default_engine = re.sub('bPackageDataInsideApk=.*$', r'bPackageDataInsideApk=False', default_engine, flags = re.M)
    default_engine = re.sub('bBuildForArmV7=.*$', r'bBuildForArmV7=True', default_engine, flags = re.M)
    default_engine = re.sub('bEnableBundle=.*$', r'bEnableBundle=True', default_engine, flags = re.M)
    default_engine = re.sub('MobileProvision=.*$', r'MobileProvision=Distro_SefaPoolDistribution.mobileprovision', default_engine, flags = re.M)
    default_engine = re.sub('SigningCertificate=.*$', r'SigningCertificate=iPhone Distribution: Connectpool ou (4H2AY6LJHW)', default_engine, flags = re.M)
    default_engine = re.sub('VersionInfo=.*$', r'VersionInfo=0.%d'%(BUILD_NUMBER), default_engine, flags = re.M)
    default_engine = re.sub('VersionDisplayName=.*$', r'VersionDisplayName=0.%d'%(BUILD_NUMBER), default_engine, flags = re.M)
    default_engine = re.sub('StoreVersion=.*$', r'StoreVersion=%d'%(BUILD_NUMBER), default_engine, flags = re.M)
print("    \033[0;32mDefaultEngine\033[0m")
with open ('Config/DefaultEngine.ini', 'w' ) as f:
    f.write(default_engine)


with open ('Config/DefaultGame.ini', 'r' ) as f:
    default_game = f.read()
    default_game = re.sub('ProjectVersion=.*$', r'ProjectVersion=0.%d'%(BUILD_NUMBER), default_game, flags = re.M)
    default_game = re.sub('ForDistribution=.*$', r'ForDistribution=True', default_game, flags = re.M)
print("    \033[0;32mDefaultGame\033[0m")
with open ('Config/DefaultGame.ini', 'w' ) as f:
    f.write(default_game)

print("  \033[0;32mConfigs patched\033[0m")
from datetime import datetime
now = datetime.now()
dt_string = now.strftime("%d/%m/%Y %H:%M:%S")


print("  \033[0;32mInjecting version info\033[0m")
with open ('Source/SefaPool/Version/Version.h', 'r' ) as f:
    version_header = f.read()
    version_header = re.sub('version = .*$', r'version = TEXT("0.%d");'%(BUILD_NUMBER), version_header, flags = re.M)
    version_header = re.sub('build = .*$', r'build = TEXT("Release build\\n%s");'%(dt_string), version_header, flags = re.M)
with open ('Source/SefaPool/Version/Version.h', 'w' ) as f:
    f.write(version_header)

print("  \033[0;32mBuilding editor\033[0m")
os.system("ue4 gen && ue4 build")

print("  \033[0;32mPackaging..\033[0m")


#print("    \033[0;32mPackaging IOS..\033[0m")
#os.system("ue4 uat -ScriptsForProject=%s/%s BuildCookRun -nocompileeditor -installed -nop4 -project=%s/%s -cook -stage -archive -archivedirectory=%s/Packaging -package -compressed -ddc=InstalledDerivedDataBackendGraph -pak -prereqs -distribution -nodebuginfo -targetplatform=IOS -build -target=%s -clientconfig=Shipping -utf8output" %
#(os.getcwd(), uproject_file, os.getcwd(), uproject_file, os.getcwd(), uproject_file[:uproject_file.find(".")]))
#print("    \033[0;32mIOS done\033[0m")

print("    \033[0;32mPackaging Android..\033[0m")
os.system("ue4 uat -ScriptsForProject=%s/%s BuildCookRun -nocompileeditor -installed -nop4 -project=%s/%s -cook -stage -archive -archivedirectory=%s/Packaging -package -compressed -ddc=InstalledDerivedDataBackendGraph -pak -prereqs -distribution -nodebuginfo -targetplatform=Android -build -target=%s -clientconfig=Shipping -utf8output -cookflavor=Multi" %
(os.getcwd(), uproject_file, os.getcwd(), uproject_file, os.getcwd(), uproject_file[:uproject_file.find(".")]))
print("    \033[0;32mAndroid done\033[0m")







