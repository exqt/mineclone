import shutil

shutil.rmtree('dist', ignore_errors=True)
shutil.copytree('assets', 'dist/assets')
shutil.copy("build/Mineclone.exe", "dist/Mineclone.exe")
shutil.copy("build/MinecloneServer.exe", "dist/MinecloneServer.exe")

print("Zipping...")
shutil.make_archive("Mineclone", "zip", "dist")
shutil.move("Mineclone.zip", "dist/Mineclone.zip")
