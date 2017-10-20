#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
from string import Template
import XilinxUpload

class HttpProc(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path != "/":
            self.send_response(301)
            self.send_header('Location','/')
            self.end_headers()
            return
        index = self.get_static_file("index.html")
        if not index:
            self.send_error(404, "Can't read index.html")
            return
        self.send_response(200)
        self.send_header('Content-Type', 'text/html')
        self.send_header("Content-Length", len(index))
        self.end_headers()
        self.wfile.write(index)

    def do_POST(self):
        # check boundary
        boundary = self.headers.plisttext.split("=")[1]
        if not boundary:
            self.send_error(404, "Content NOT begin with boundary")
            return
        # get content size
        size = int(self.headers['content-length'])
        # upload bitstream
        upload_result = None
        with open("bitstream.dump", "wb") as bitstream:
            data = self.rfile.read(size)
            bitstream.write(data)
            upload_result = XilinxUpload.dump(data)
        if not upload_result:
            self.send_error(404, "XilinxUpload error")
            return
        # send result
        result = self.get_static_file("result.html")
        if not result:
            self.send_error(404, "Can't read result.html")
            return
        result = Template(result).substitute(
                filename = upload_result['NCDFilename'],
                device = upload_result['device'],
                date = upload_result['date'],
                time = upload_result['time'],
                length = upload_result['length'],
                referer = self.headers['referer'])
        self.send_response(200)
        self.send_header("Content-Type", "text/html")
        self.send_header("Content-Length", len(result))
        self.end_headers()
        self.wfile.write(result)

    def get_static_file(self, name):
        content = None
        with open("static/" + name, "r") as f:
            content = f.read()
        return content

if __name__ == '__main__':
    addr = ("0.0.0.0", 3000)
    print "Server running on", addr
    serv = HTTPServer(addr, HttpProc)
    serv.serve_forever()

