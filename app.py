from flask import Flask, render_template, request, redirect, flash, session
import subprocess
import os

app = Flask(__name__)
app.secret_key = "ftpsecret"

UPLOAD_FOLDER = "./uploads"
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

@app.route('/', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        ip = request.form['ip']
        user = request.form['username']
        passwd = request.form['password']
        session['ip'] = ip
        session['user'] = user
        session['pass'] = passwd

        try:
            result = subprocess.run(
                ['./client2', 'connect', ip, user, passwd],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                timeout=10
            )
            if "230" in result.stdout or "Login successful" in result.stdout:
                flash("Connected successfully.")
                return redirect('/dashboard')
            else:
                flash("Login failed: " + result.stdout)
                return redirect('/')
        except Exception as e:
            flash(f"Error: {e}")
            return redirect('/')

    return render_template('login_new.html')

@app.route('/dashboard')
def dashboard():
    return render_template('dashboard_down.html')

@app.route('/upload', methods=['POST'])
def upload():
    if 'ip' not in session or 'user' not in session or 'pass' not in session:
        flash("You must login first.")
        return redirect('/')

    file = request.files.get('file')
    if not file or file.filename == '':
        flash("No file selected.")
        return redirect('/dashboard')

    filepath = os.path.join(app.config['UPLOAD_FOLDER'], file.filename)
    file.save(filepath)

    try:
        result = subprocess.run(
            ['./client2', 'connect_upload', session['ip'], session['user'], session['pass'], filepath],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            timeout=60
        )
        flash(result.stdout)
    except Exception as e:
        flash(f"Upload failed: {e}")

    return redirect('/dashboard')

@app.route('/download', methods=['POST'])
def download():
    if 'ip' not in session or 'user' not in session or 'pass' not in session:
        flash("You must login first.")
        return redirect('/')

    filename = request.form.get('filename')
    if not filename:
        flash("No filename provided.")
        return redirect('/dashboard')

    try:
        result = subprocess.run(
            ['./client2', 'connect_download', session['ip'], session['user'], session['pass'], filename],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            timeout=60
        )
        flash(result.stdout)
    except Exception as e:
        flash(f"Download failed: {e}")

    return redirect('/dashboard')
 
@app.route('/list')
def list_files():
    if 'ip' not in session or 'user' not in session or 'pass' not in session:
        flash("You must login first.")
        return redirect('/')

    try:
        result = subprocess.run(
            ['./client2', 'connect_list', session['ip'], session['user'], session['pass']],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            timeout=10
        )
        file_lines = result.stdout.strip().splitlines()
        
        files = []
        for line in file_lines:
            parts = line.split()
            if len(parts) >= 9 :  
                files.append(parts[-1]) 
        return render_template('file_browser.html', files=files)
    except Exception as e:
        flash(f"Error listing files: {e}")
        return redirect('/dashboard')

if __name__ == '__main__':
    app.run(debug=True)

