# Clear
rm -f ./Watercolor.7z

# Build 
docker image build -t watercolor .
docker container create --name watercolor watercolor

# Copy
docker cp watercolor:/Watercolor.7z ./Watercolor.7z

# Clear
docker container rm watercolor
docker image rm watercolor