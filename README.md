# 0xColor
X11 application which:
* Parses color names, in either hex format or X11 color names
* Displays the colors
* Prints to standard output, the names of all the colors sucessfully read and other information such as the total number of colors that it tried to parse versus how many were actually valid

## Usage
The color names are read through the **standard input**, and cant be **piped** to **hexprev** or **redirected** into **hexprev** <br>
***example*** <br>
`hexprev < colors.txt`
where the `colors.txt` file might contain the following text
```
#232323
red
pink
hot pink
#452367
this line text will not be read but it will try to find all hex codes in lines that aren't read correctly, #343434 #659090
rgb:1.0/0.6/1.8

```
`hexprev < ~/.Xresources` This line can be used to preview all the colors in the Xresources file.
`echo "red\n" "green\n" | hexprev` Pipes in the strings 'red' 'green', the newline is important.
`hexprev` You can also simply run the command by itself, and you will be able to enter colors and then entering EOF (CTRL - D) will terminate input and display the colors.
running `hexprev` then entering 'help' will display on how to use hexprev and the different options.

This application is complete in the fact that it does on thing (accept colors from stdin and display them in an X11 application) and it does it reasonably well. But i would like to add more features once i have read through the entire X11 manual and learned more.
