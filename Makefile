.PHONY: clean

clean:
	$(RM) -r build dublintraceroute/*.so .cache .coverage \
		dublintraceroute.egg-info/ .eggs \
		./tests/__pycache__ ./dublintraceroute/__pycache__
