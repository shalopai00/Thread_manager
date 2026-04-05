# visualizer_simple.py
import subprocess
import time
import tkinter as tk
from tkinter import ttk
import os


class ThreadMonitor:
    def __init__(self, process_name="OpenMP_for_laba5"):
        self.process_name = process_name
        self.pid = None
        self.running = True

        self.root = tk.Tk()
        self.root.title(f"Thread Monitor - {process_name}")
        self.root.geometry("1000x600")

        self.create_widgets()
        self.find_process()
        self.update_display()

    def create_widgets(self):
        # Информационная панель
        info_frame = ttk.LabelFrame(self.root, text="Process Info", padding=10)
        info_frame.pack(fill=tk.X, padx=10, pady=5)

        self.pid_label = ttk.Label(info_frame, text="PID: --", font=("Arial", 12))
        self.pid_label.pack(side=tk.LEFT, padx=10)

        self.status_label = ttk.Label(info_frame, text="Status: Searching...", font=("Arial", 12))
        self.status_label.pack(side=tk.LEFT, padx=10)

        self.threads_label = ttk.Label(info_frame, text="Threads: 0", font=("Arial", 12))
        self.threads_label.pack(side=tk.LEFT, padx=10)

        # Таблица потоков
        tree_frame = ttk.LabelFrame(self.root, text="Threads", padding=10)
        tree_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        columns = ("TID", "Name", "CPU%", "State")
        self.tree = ttk.Treeview(tree_frame, columns=columns, show="headings", height=15)

        self.tree.heading("TID", text="Thread ID")
        self.tree.heading("Name", text="Name")
        self.tree.heading("CPU%", text="CPU %")
        self.tree.heading("State", text="State")

        self.tree.column("TID", width=100)
        self.tree.column("Name", width=300)
        self.tree.column("CPU%", width=100)
        self.tree.column("State", width=150)

        scrollbar = ttk.Scrollbar(tree_frame, orient=tk.VERTICAL, command=self.tree.yview)
        self.tree.configure(yscrollcommand=scrollbar.set)

        self.tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        # Статистика
        stats_frame = ttk.LabelFrame(self.root, text="Statistics", padding=10)
        stats_frame.pack(fill=tk.X, padx=10, pady=5)

        self.total_cpu_label = ttk.Label(stats_frame, text="Total CPU: 0%", font=("Arial", 11, "bold"))
        self.total_cpu_label.pack(side=tk.LEFT, padx=20)

        self.active_label = ttk.Label(stats_frame, text="Active: 0", font=("Arial", 11))
        self.active_label.pack(side=tk.LEFT, padx=20)

        self.progress = ttk.Progressbar(stats_frame, length=300, mode='determinate')
        self.progress.pack(side=tk.LEFT, padx=20)

    def find_process(self):
        try:
            result = subprocess.run(['pgrep', '-f', self.process_name],
                                    capture_output=True, text=True)
            if result.stdout:
                self.pid = int(result.stdout.strip().split('\n')[0])
                self.pid_label.config(text=f"PID: {self.pid}")
                self.status_label.config(text="Status: Running")
                return True
        except:
            pass
        return False

    def get_thread_info(self):
        if not self.pid:
            return []

        try:
            os.kill(self.pid, 0)
        except OSError:
            self.pid = None
            return []

        threads = []
        try:

            result = subprocess.run(['ps', '-T', '-p', str(self.pid),
                                     '-o', 'tid,comm,pcpu,state'],
                                    capture_output=True, text=True)
            lines = result.stdout.strip().split('\n')[1:]

            for line in lines:
                if line.strip():
                    parts = line.split()
                    if len(parts) >= 4:
                        threads.append({
                            'tid': parts[0],
                            'name': parts[1],
                            'cpu': float(parts[2]),
                            'state': parts[3]
                        })
        except:
            pass

        return threads

    def update_display(self):
        if not self.running:
            return

        if not self.pid:
            if self.find_process():
                self.update_display()
            else:
                self.status_label.config(text="Status: Waiting...")
                self.root.after(1000, self.update_display)
            return

        threads = self.get_thread_info()

        if threads:

            for item in self.tree.get_children():
                self.tree.delete(item)

            total_cpu = 0
            active = 0

            state_map = {'R': 'Running', 'S': 'Sleeping',
                         'D': 'Waiting', 'Z': 'Zombie'}

            for t in threads:
                tid = t['tid']
                name = t['name'][:40]
                cpu = t['cpu']
                state = state_map.get(t['state'], t['state'])

                total_cpu += cpu
                if cpu > 5:
                    active += 1


                if 'scheduler' in name.lower():
                    display_name = f"[SCHED] {name}"
                elif 'worker' in name.lower():
                    display_name = f"[WORK] {name}"
                elif 'main' in name.lower():
                    display_name = f"[MAIN] {name}"
                else:
                    display_name = name

                self.tree.insert("", tk.END, values=(tid, display_name, f"{cpu:.1f}", state))


            self.threads_label.config(text=f"Threads: {len(threads)}")
            self.total_cpu_label.config(text=f"Total CPU: {total_cpu:.1f}%")
            self.active_label.config(text=f"Active: {active}")
            self.progress['value'] = min(total_cpu, 100)

            self.status_label.config(text="Status: Running")
        else:
            self.status_label.config(text="Status: No threads found")

        self.root.after(1000, self.update_display)

    def run(self):
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.root.mainloop()

    def on_closing(self):
        self.running = False
        self.root.destroy()


if __name__ == "__main__":
    monitor = ThreadMonitor()
    monitor.run()