rem del /Q cache\*
call vcvars32
cd ogre
python generate_code.py >1
cd ..
cd ogreode
python generate_code.py >1
cd ..
cd ogrenewt
python generate_code.py >1
cd ..
cd ogreal
python generate_code.py >1
cd ..
cd ogrerefapp
python generate_code.py >1
cd ..
cd ode
python generate_code.py >1
cd ..
cd ois
python generate_code.py >1
cd ..
cd cegui
python generate_code.py >1
cd ..
cd quickgui
python generate_code.py >1
cd ..
cd opcode
python generate_code.py >1
cd ..
cd physx
python generate_code.py >1
cd ..
cd nxogre
python generate_code.py >1
cd ..
cd ogredshow
python generate_code.py >1
cd ..
cd ogrevideoffmpeg
python generate_code.py >1
cd ..
cd theora
python generate_code.py >1
cd ..
cd bullet
python generate_code.py >1
cd ..
cd betagui
python generate_code.py >1
cd ..
cd plib
python generate_code.py >1
cd ..
cd ..
#scons -i
