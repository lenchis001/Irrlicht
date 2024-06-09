# Clear
rm -f ./Watercolor.zip  

# Build 
docker image build -t watercolor --platform linux/amd64 .
docker container create --name watercolor watercolor

# Copy
docker cp watercolor:/Watercolor.zip ./Watercolor.zip

# Clear
docker container rm watercolor
docker image rm watercolor