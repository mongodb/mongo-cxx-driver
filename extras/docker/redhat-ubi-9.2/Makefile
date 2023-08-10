MONGOCXX_VERSION=3.8.0

.PHONY: build
build:
	docker build . \
		-t mongo-cxx-driver:$(MONGOCXX_VERSION)-redhat-ubi-9.2 \
		--build-arg="MONGOCXX_VERSION=$(MONGOCXX_VERSION)"

.PHONY: cleanbuild
cleanbuild:
	docker build . \
		--no-cache \
		-t mongo-cxx-driver:$(MONGOCXX_VERSION)-redhat-ubi-9.2 \
		--build-arg="MONGOCXX_VERSION=$(MONGOCXX_VERSION)"

.PHONY: run
run:
	docker run --rm -it mongo-cxx-driver:$(MONGOCXX_VERSION)-redhat-ubi-9.2

.PHONY: test
test:
	docker build . --no-cache -f TestDockerfile -t test-redhat-ubi-mongocxx-driver-image
	docker run test-redhat-ubi-mongocxx-driver-image
