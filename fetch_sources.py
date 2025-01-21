import os
import subprocess
import argparse

EXTERNAL_DIR = os.path.join(os.path.realpath(os.path.normpath(os.path.dirname(__file__))), "external")

g_workDirStack = []

def pushd(path):
	oldDir = os.getcwd()
	os.chdir(path)
	g_workDirStack.append(oldDir)

def popd():
	assert len(g_workDirStack) > 0
	newDir = g_workDirStack[-1]
	g_workDirStack.pop()
	os.chdir(newDir)

def execute(args):
	retcode	= subprocess.call(args)
	if retcode != 0:
		raise Exception("Failed to execute '%s', got %d" % (str(args), retcode))

class GitSource:
	def __init__(self, httpsUrl: str, baseDirectory: str, branch: str = None):
		self.httpsUrl = httpsUrl
		self.baseDirectory = baseDirectory
		self.branch = branch

	def fetch(self, force: bool):
		sourceDirectory = os.path.join(EXTERNAL_DIR, self.baseDirectory)
		if not os.path.exists(sourceDirectory):
			os.mkdir(sourceDirectory)

		if not os.path.exists(os.path.join(sourceDirectory, '.git')):
			execute(["git", "clone", self.httpsUrl, sourceDirectory])

		pushd(sourceDirectory)
		try:
			force_arg = ['--force'] if force else []
			execute(["git", "fetch"] + force_arg + ["--tags", self.httpsUrl, "+refs/heads/*:refs/remotes/origin/*"])

			if self.branch:
				execute(["git", "checkout"] + force_arg + [f"origin/{self.branch}"])
			else:
				execute(["git", "checkout"] + force_arg + ["origin/HEAD"])
		finally:
			popd()

SOURCES = [
	GitSource("https://github.com/ocornut/imgui.git",               "imgui", "docking"),
	GitSource("https://github.com/CedricGuillemet/ImGuizmo.git",    "imguizmo"),
	GitSource("https://github.com/libsdl-org/SDL.git",              "SDL"),
	GitSource("https://github.com/nlohmann/json.git",               "JSON"),
	GitSource("https://github.com/g-truc/glm.git",                  "glm"),
	GitSource("https://github.com/tinyobjloader/tinyobjloader.git", "tinyobj"),
	GitSource("https://github.com/nothings/stb.git",                "stb"),
	GitSource("https://github.com/cginternals/glbinding.git",       "glbinding"),
	GitSource("https://github.com/freetype/freetype.git",           "FreeType"),
]

def read_arguments():
	parser = argparse.ArgumentParser(description='Process some integers.')
	parser.add_argument("--clear", dest="clear", action="store_true", default=False,
						help="Remove parent folder where sources are fetched to")
	parser.add_argument("--force", dest="force", action="store_true", default=False,
						help="Pass --force to git fetch and checkout source")

	return parser.parse_args()

def fetch_sources(force: bool):
	if not os.path.exists(EXTERNAL_DIR):
		os.mkdir(EXTERNAL_DIR)

	# Fetch sources
	for source in SOURCES:
		source.fetch(force)

if __name__ == "__main__":
	args = read_arguments()

	if args.clear:
		os.rmdir(EXTERNAL_DIR)
	else:
		fetch_sources(args.force)
