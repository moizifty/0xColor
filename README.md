# 0xColor
X11 application which:
* Parses color names, in either hex format or X11 color names
* Displays the colors
* Prints to standard output, the names of all the colors sucessfully read and other information such as the total number of colors that it tried to parse versus how many were actually valid

# Installing
`git clone https://github.com/AwkwardBanana/0xColor` to clone the repo
`sudo make install && sudo make clean`

## Usage
The color names are read through the **standard input**, and cant be **piped** to **0xColor** or **redirected** into **0xColor** <br>
***example*** <br>
`0xColor < colors.txt`
where the `colors.txt` file might contain the following text
```/
#232323
red
pink
hot pink
#452367
this line text will not be read but it will try to find all hex codes in lines that aren't read correctly, #343434 #659090
rgb:1.0/0.6/1.8

```
`0xColor < ~/.Xresources` This line can be used to preview all the colors in the Xresources file.
`echo "red\n" "green\n" | 0xColor` Pipes in the strings 'red' 'green', the newline is important.
`0xColor` You can also simply run the command by itself, and you will be able to enter colors and then entering EOF (CTRL - D) will terminate input and display the colors.
running `0xColor` then entering 'help' will display on how to use 0xColor and the different options.

This application is complete in the fact that it does on thing (accept colors from stdin and display them in an X11 application) and it does it reasonably well. But i would like to add more features once i have read through the entire X11 manual and learned more.
