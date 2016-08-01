# ROFI_COLUMNS 1 rofi_columns

## NAME

**rofi_columns** - An UTF-8 columnate lists tool

## SYNOPSIS

**rofi_columns** [-iem] [-s split_regex] [-f output format] [-i input] [-o output]

## Options

**-m**

 Use matching instead of splitting.

**-e**

 Pango escape the fields in the output.

**-i**

 Output statistics off the input.

**-f** *format*

 Format the output.

**-s** *regex*

 Regex used for splitting or matching.

**-i** *input file*

 Read from file instead of *stdin*.

**-o** *output_file*

 Write to file instead of *stdout*.

## FORMAT

The output is formatted space separated by default, where each column is the width of the maximum field.
It can be customized, by passing a string to **-f**. In this string, ```{column:length}``` gets replaced.

**column** starts counting at 1.

**length** can be:

* -1: Don't pad the column
*  0: Pad the column to the maximum length of the field.
* \>0: Pad/trim the column to be **length* width.

The width will not deterimened on the byte length, but actual characters. So column should still line up correctly when
it involves UTF-8 characters. Also when **-e** is passed it does not count escaping characters. 

## Example

Give nicely structured and markupped list of manpages in rofi.

```man -s3,4 -k . | ./rofi_columns  -s "^([\S]+)\s\((\w+)\)\s+- (.*)$"  -m -f "<i>{2:4}</i>i> <b>{1:15}</b>b> {3:-1}" -e  | rofi -dmenu -markup-rows```


## LICENSE

    MIT/X11

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## WEBSITE

**rofi_columns** website can be found at [here](https://davedavenport.github.io/rofi_columns/)

**rofi_columns** bugtracker can be found [here](https://github.com/DaveDavenport/rofi_columns/issues)

**rofi_columns** support can be obtained [here](irc://irc.freenode.net/#rofi) (#rofi on irc.freenode.net)

## AUTHOR

Qball Cow <qball@gmpclient.org>
