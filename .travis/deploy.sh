rm -rf gh-pages
sudo -E docker run --name docker_deploy $CONAN_DOCKER_IMAGE /bin/sh -c "conan remote add conan-mpusz https://api.bintray.com/conan/mpusz/conan-mpusz"
sudo -E docker exec docker_deploy /bin/sh -c "conan search -r conan-mpusz mp-units --raw | tail -n1 | xargs -I % conan install % -g deploy -s compiler.version=$CONAN_GCC_VERSIONS -s compiler.cppstd=20"
sudo -E docker cp docker_deploy:/home/conan/mp-units/share/doc/units/sphinx gh-pages
