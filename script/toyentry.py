print('hello from toyentry.py')

import gameapp

        
app = gameapp.App()

def startup():
    print('py startup')
    app.startup()
    
def update():
    app.update()

def shutdown():
    print('py shutdown')
    app.shutdown()

