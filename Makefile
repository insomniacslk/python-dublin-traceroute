.PHONY: clean test

test: test-py3 test-py2

build-ext-py3:
	 python3 setup.py build_ext --inplace

build-ext-py2:
	 python setup.py build_ext --inplace

test-py3: build-ext-py3
	py.test-3 -v --cov --cov-report term-missing

test-py2: build-ext-py2
	py.test -v --cov --cov-report term-missing

clean:
	$(RM) -r build dublintraceroute/*.so .cache .coverage
