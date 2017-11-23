#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from __future__ import print_function
import sys, os
import requests

def upload(fileName):
    url = 'http://127.0.0.1:3000'
    files = {fileName: open(fileName, 'rb')}
    return requests.post(url=url, files=files)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: %s bitstream.bit" % sys.argv[0])
        sys.exit(1)
    bitstream = sys.argv[1]
    try:
        res = upload(bitstream)
        print("Status code: %d" % res.status_code)
        if res.status_code != 200:
            sys.exit(1)
    except Exception as e:
        print("Can't upload %s" % bitstream)
        print(e)
        sys.exit(1)
