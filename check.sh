#!/bin/bash
(sleep 0.05; ./bin/sw_plain ./config/alice.json > ans1) &  ./bin/sw_plain ./config/bob.json >ans2

(sleep 0.05; ./bin/sw ./config/alice.json > res1) &  ./bin/sw ./config/bob.json >res2

if diff ans1 res1; then
	echo Alice,Ok
else
	echo Alice,Wrong!
fi

if diff ans2 res2; then
	echo Bob,Ok
else
	echo Bob,Wrong!
fi

rm ans1
rm ans2
rm res1
rm res2
