# -*- coding: utf-8 -*-
# @Author: yuan.jiang
# @Date:   2019-01-11 14:38:48
# @Last Modified by:   yuan.jiang
# @Last Modified time: 2019-01-11 15:15:49
import io
import formatter
# from htmllib import HTMLParser
from html.parser import HTMLParser   # 不能用？？？
import http.client
import os
import sys
import urllib.request, urllib.parse, urllib.error
import urllib.parse

class Retriever(object):
    __slots__ =('url', 'file')
    def __init__(self, url):
        self.url, self.file = self.get_file(url)

    # 获取URL和文件路径
    def get_file(self, url, default='index.html'):
        'Create usable local filename from URL'
        parsed = urllib.parse.urlparse(url)
        host = parsed.netloc.split('@')[-1].split(':')[0]
        filepath = '%s%s' % (host, parsed.path)
        if not os.path.splitext(parsed.path)[1]:
            filepath = os.path.join(filepath, default)
        linkdir = os.path.dirname(filepath)
        if not os.path.isdir(linkdir):
            if os.path.exists(linkdir):
                os.unlink(linkdir)
            os.makedirs(linkdir)
        return url, filepath

    # 下载URL的html内容到文件
    def download(self):
        'Download URL to specific named file'
        try:
            retval = urllib.request.urlretrieve(self.url, self.file)
        except (IOError, http.client.InvalidURL) as e:
            retval = (('*** ERROR: bad URL "%s": %s' % (self.url, e)))
        return retval

    # 从包含HTML信息的文件中解析连接，返回列表
    def parse_links(self):
        'Parse out the links found in downloaded HTML file'
        f = open(self.file, 'r')
        data = f.read()
        f.close()
        parser = HTMLParser(formatter.AbstractFormatter(formatter.DumbWriter(io.StringIO())))
        parser.feed(data)
        parser.close()
        return parser.anchorlist

class Crawler(object):
    count = 0
    def __init__(self, url):
        self.q = [url]
        self.seen = set()
        parsed = urllib.parse.urlparse(url)
        host = parsed.netloc.split('@')[-1].split(':')[0]
        self.dom = '.'.join(host.split('.')[-2:])

    # 获取URL中实际网页内容及解析连接
    def get_page(self, url, media=False):
        'Download page & parse links, add to queue if nec'
        r = Retriever(url)
        fname = r.download()[0]
        if fname[0] == '*':
            print((fname, '... skipping parse'))
            return
        Crawler.count += 1
        print(('\n(', Crawler.count, ')'))
        print(('URL:', url))
        print(('FILE:', fname))
        self.seen.add(url)
        ftype = os.path.splitext(fname)[1]
        if ftype not in ('.htm', '.html'):
            return

        for link in r.parse_links():
            if link.startswith('mailto:'):
                print ('... discarded, mailto link')
                continue
            if not media:
                ftype = os.path.splitext(link)[1]
                if ftype in ('.mp3', '.mp4', '.m4v', '.wav'):
                    print ('... discarded, media file')
                    continue
            if not link.startswith('http://'):
                link = urllib.parse.urljoin(url, link)  #根路径组newurl
            print(('*', link))
            if link not in self.seen:
                if self.dom not in link:
                    print ('... discarded, not in domain')
                else:
                    if link not in self.q:
                        self.q.append(link)
                        print ('... new, added to Q')
                    else:
                        print ('... discarded, already in Q')
            else:
                print ('... discarded, already processed')

    #遍历下一个页面
    def go(self, media=False):
        'Process next page in queue(if any)'
        while self.q:
            url = self.q.pop()
            self.get_page(url, media)

def main():
    if len(sys.argv) > 1:
        url = sys.argv[1]
    else:
        try:
            url = input('Enter starting URL:')
        except(KeyboardInterrupt, EOFError):
            url = ''
    if not url:
        return
    if not url.startswith('http://') and not url.startswith('ftp://'):
        url = 'http://%s/' % url
    robot = Crawler(url)
    robot.go()

if __name__ == '__main__':
    main()