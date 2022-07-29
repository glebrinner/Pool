python -m ue4cli package Development -server -noclient -serverplatform=linux
docker build . -f .\Build\LinuxServer\DockerfileAlreadyBuilt -t staging-registry.gotbitgames.co/sefapooluev2:dima
docker push staging-registry.gotbitgames.co/sefapooluev2:dima
kubectl delete gs --all
pause