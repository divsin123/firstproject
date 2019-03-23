import Tkinter
import os        
from Tkinter import *
from tkMessageBox import *
from tkFileDialog import *
from tkFont import *

class Notepad: 

        root = Tk()
        w=Font()

        thisWidth = 300
        thisHeight = 300
        thisTextArea = Text(root) 
        thisMenuBar = Menu(root) 
        thisFileMenu = Menu(thisMenuBar, tearoff=0) 
        thisEditMenu = Menu(thisMenuBar, tearoff=0) 
        thisHelpMenu = Menu(thisMenuBar, tearoff=0)
        thisFormatMenu = Menu(thisMenuBar, tearoff=0)
        thisFontMenu=Menu(thisMenuBar, tearoff=0)
        font_menu=Menu(thisMenuBar, tearoff=0)
        
        thisScrollBar = Scrollbar(thisTextArea)  
        file = None

        def font(self):
                self.textfont=StringVar()
                #self.text_font.set("Times")

                
                self.font={}
                for font in ("Arial","Consoles","Courier","tahoma"):
                        self.fonts[font]=tkinter.font.Font(font=font)
                        font_menu.add_radiobutton(label=font,variable=self.text_font,command=self.change_font)
                self.thisMenuBar.add_cascade(lable="font",menu=self.thisfontMenu)
                root.config(menu=the_menu)
                def change_font(self):
                        self.text_area.config(font=self.fonts[self.text_font.get()])

                '''self.tkFont.Font(family="Times",size=10,weight=BOLD)
                self.tkFont.Font(family="Helvetica",size=10,weight=BOLD,slant=ITALIC)
                self.tkFont.Font(family="Symbol",size=8,weight=BOLD)'''
        
                

        def __init__(self,**kwargs):

                
                try: 
                                self.root.wm_iconbitmap("Notepad.ico") 
                except: 
                                pass
 

                try: 
                        self.thisWidth = kwargs['width'] 
                except KeyError: 
                        pass

                try: 
                        self.thisHeight = kwargs['height'] 
                except KeyError: 
                        pass 
                self.root.title("Unsaved - Notepad") 

                screenWidth = self.root.winfo_screenwidth() 
                screenHeight = self.root.winfo_screenheight()
                
                left = (screenWidth / 2) - (self.thisWidth / 2) 
                 
                top = (screenHeight / 2) - (self.thisHeight /2) 
                
                self.root.geometry('%dx%d+%d+%d' % (self.thisWidth,self.thisHeight,left, top)) 
 
                self.root.grid_rowconfigure(0, weight=1) 
                self.root.grid_columnconfigure(0, weight=1) 

                self.thisTextArea.grid(sticky = N + E + S + W) 
                 
                self.thisFileMenu.add_command(label="New",command=self.newFile)  
                
                self.thisFileMenu.add_command(label="Open",command=self.openFile) 
                 
                self.thisFileMenu.add_command(label="Save",command=self.saveFile)        
                 
                self.thisFileMenu.add_separator()                                                                                
                self.thisFileMenu.add_command(label="Exit",command=self.quitApplication) 
                self.thisMenuBar.add_cascade(label="File",menu=self.thisFileMenu)        
                 
                self.thisEditMenu.add_command(label="Cut",command=self.cut)                    
                 
                self.thisEditMenu.add_command(label="Copy",command=self.copy)          
                 
                self.thisEditMenu.add_command(label="Paste",command=self.paste)                
                
                self.thisMenuBar.add_cascade(label="Edit",menu=self.thisEditMenu)        

                                 
                #self.thisFormatMenu.add_command(label="font",command=self.w)                            
                
                #self.thisMenuBar.add_cascade(label="Format",menu=self.thisFormatMenu)
                self.thisFontMenu.add_command(label="Verdana", command=self.changeFont)
                #self.thisMenuBar.add_cascade(label="Font",menu=self.thisFontMenu)
                self.thisFontMenu.add_command(label="x", command=self.changeFont1)
                #self.thisMenuBar.add_cascade(label="Font",menu=self.thisFontMenu)
                self.thisFontMenu.add_command(label="Arial", command=self.changeFont2       )
                self.thisMenuBar.add_cascade(label="Font",menu=self.thisFontMenu)
                
                
                self.root.config(menu=self.thisMenuBar) 

                self.thisScrollBar.pack(side=RIGHT,fill=Y)                                     
                

                
                self.thisHelpMenu.add_command(label="About Notepad",command=self.showAbout) 
                self.thisMenuBar.add_cascade(label="Help",menu=self.thisHelpMenu) 

                self.root.config(menu=self.thisMenuBar) 

                self.thisScrollBar.pack(side=RIGHT,fill=Y)                                       
                         
                self.thisScrollBar.config(command=self.thisTextArea.yview)       
                self.thisTextArea.config(yscrollcommand=self.thisScrollBar.set) 
        
                
        def quitApplication(self): 
                self.root.destroy() 
                # exit() 

        def showAbout(self): 
                showinfo("Notepad","This is a text editor project using tkinter") 

        def openFile(self): 
                
                self.file = askopenfilename(defaultextension=".txt",filetypes=[("All Files","*.*"),("Text Documents","*.txt")]) 

                if self.file == "": 
                        
                        self.file = None
                else: 
                        
                        self.root.title(os.path.basename(self.file) + " - Notepad") 
                        self.thisTextArea.delete(1.0,END) 

                        file = open(self.file,"r") 

                        self.thisTextArea.insert(1.0,file.read()) 

                        file.close() 

                
        def newFile(self): 
                self.root.title("Untitled - Notepad") 
                self.file = None
                self.thisTextArea.delete(1.0,END) 

        def saveFile(self): 

                if self.file == None: 
                        self.file = asksaveasfilename(initialfile='Untitled.txt',defaultextension=".txt",filetypes=[("All Files","*.*"),("Text Documents","*.txt")]) 

                        if self.file == "": 
                                self.file = None
                        else: 
                                 
                                file = open(self.file,"w") 
                                file.write(self.thisTextArea.get(1.0,END)) 
                                file.close() 
                        
                                self.root.title(os.path.basename(self.file) + " - Notepad") 
                                
                        
                else: 
                        file = open(self.file,"w") 
                        file.write(self.thisTextArea.get(1.0,END)) 
                        file.close() 

        def cut(self): 
                self.thisTextArea.event_generate("<<Cut>>") 

        def copy(self): 
                self.thisTextArea.event_generate("<<Copy>>") 

        def paste(self): 
                self.thisTextArea.event_generate("<<Paste>>")
        def changeFont(self):
                self.bold_font = Font(family="Verdana", size=14, weight="bold")#Applied bold just to see the difference.Otherwise use weight="normal"
                try:
                        print ("Inside try")
                        self.thisTextArea.tag_add("Verdana", "sel.first", "sel.last")
                        print(self.thisTextArea.selection_get())
                        self.thisTextArea.tag_configure("Verdana",font=self.bold_font)
                        print ("Font chagned")
                except tkinter.TclError:
                        print ("Exception")
        def changeFont1(self):
                self.bold_font = Font(family="Verdana", size=14, weight="bold")#Applied bold just to see the difference.Otherwise use weight="normal"
                try:
                        print ("Inside try")
                        self.thisTextArea.tag_add("Verdana", "sel.first", "sel.last")
                        print(self.thisTextArea.selection_get())
                        self.thisTextArea.tag_configure("Verdana",font=self.bold_font)
                        print ("Font chagned")
                except tkinter.TclError:
                        print ("Exception")
        def changeFont2(self):
                self.italic_font = Font(family="Arial", size=24, weight="normal")#Applied bold just to see the difference.Otherwise use weight="normal"
                try:
                        print ("Inside try")
                        self.thisTextArea.tag_add("Arial", "sel.first", "sel.last")
                        print(self.thisTextArea.selection_get())
                        self.thisTextArea.tag_configure("Arial",font=self.italic_font)
                        print ("Font chagned")
                except tkinter.TclError:
                        print ("Exception")
        

        def run(self): 
 
                self.root.mainloop() 


notepad = Notepad(width=600,height=400) 
notepad.run()


