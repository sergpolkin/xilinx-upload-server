#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import os
import shutil
import XilinxUpload

class HttpProc(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path != "/":
            self.send_response(301)
            self.send_header('Location','/')
            self.end_headers()
            return
        f = None
        try:
            f = open("static/index.html", "rb")
        except IOError:
            self.send_error(404, "Can't read index.html")
            return
        self.send_response(200)
        self.send_header('Content-Type', 'text/html')
        fs = os.fstat(f.fileno())
        self.send_header("Content-Length", str(fs[6]))
        self.end_headers()
        shutil.copyfileobj(f, self.wfile)
        f.close()

    def do_POST(self):
        # check boundary
        boundary = self.headers.plisttext.split("=")[1]
        if not boundary:
            self.send_error(404, "Content NOT begin with boundary")
            return
        # get content size
        size = int(self.headers['content-length'])
        with open("bitstream.dump", "wb") as bitstream:
            data = self.rfile.read(size)
            bitstream.write(data)
            XilinxUpload.dump(data)
        # send result
        self.send_response(200)
        self.send_header("Content-Type", "text/html")
        self.end_headers()
        f = self.wfile
        f.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">")
        f.write("<html>\n<title>Upload Result Page</title>\n")
        f.write("<body>\n<p>Upload size: %d</p>\n" % size)
        f.write("<br><a href=\"%s\">back</a>" % self.headers['referer'])
        f.write("</body>\n</html>\n")

if __name__ == '__main__':
    addr = ("0.0.0.0", 3000)
    print "Server running on", addr
    serv = HTTPServer(addr, HttpProc)
    serv.serve_forever()

