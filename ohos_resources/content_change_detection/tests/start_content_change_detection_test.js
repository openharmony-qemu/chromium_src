const fs = require('fs');
const path = require('path');

console.log('Starting Viewport Font Monitor Tests...\n');

const scriptPath = path.join(__dirname, '../dist/start_content_change_detection.js');
let scriptContent = '';
try {
  scriptContent = fs.readFileSync(scriptPath, 'utf8');
  console.log('Script file loaded successfully');
} catch (error) {
  console.error('Failed to load script:', error.message);
  process.exit(1);
}

const htmlTemplate = `
<!DOCTYPE html>
<html>
<head>
    <title>Viewport Monitor Tests</title>
    <style>
        * { box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            margin: 0;
            padding: 20px;
            background: #f5f5f5;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            background: white;
            border-radius: 8px;
            padding: 30px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            border-bottom: 2px solid #4CAF50;
            padding-bottom: 10px;
        }
        .test-section {
            margin: 25px 0;
            padding: 20px;
            background: #f9f9f9;
            border-radius: 6px;
            border-left: 4px solid #2196F3;
        }
        .test-item {
            margin: 12px 0;
            padding: 12px;
            background: white;
            border-radius: 4px;
            border: 1px solid #e0e0e0;
        }
        .test-pass {
            color: #4CAF50;
            border-left: 4px solid #4CAF50;
        }
        .test-fail {
            color: #f44336;
            border-left: 4px solid #f44336;
        }
        .test-error {
            color: #ff9800;
            background: #fff3e0;
            padding: 10px;
            border-radius: 4px;
            margin: 5px 0;
            font-family: monospace;
            font-size: 12px;
        }
        .summary {
            margin-top: 30px;
            padding: 20px;
            background: #e8f5e9;
            border-radius: 6px;
            text-align: center;
        }
        .summary-pass { color: #2e7d32; font-weight: bold; }
        .summary-fail { color: #c62828; font-weight: bold; }
        button {
            background: #2196F3;
            color: white;
            border: none;
            padding: 12px 24px;
            border-radius: 4px;
            cursor: pointer;
            font-size: 16px;
            margin: 10px 5px;
            transition: background 0.3s;
        }
        button:hover { background: #1976D2; }
        button:disabled {
            background: #ccc;
            cursor: not-allowed;
        }
        .console {
            background: #263238;
            color: #eceff1;
            padding: 15px;
            border-radius: 4px;
            margin-top: 20px;
            font-family: 'Courier New', monospace;
            font-size: 12px;
            max-height: 200px;
            overflow-y: auto;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Viewport Font Monitor Tests</h1>
        
        <div class="test-section">
            <h2>Test Results</h2>
            <div id="test-results"></div>
        </div>
        
        <div class="test-section">
            <h2>Controls</h2>
            <button id="run-tests">Run Tests</button>
            <button id="reset-tests">Reset Tests</button>
        </div>
        
        <div class="test-section">
            <h2>Console Output</h2>
            <div id="console-output" class="console"></div>
        </div>
        
        <div id="summary" class="summary" style="display: none;"></div>
    </div>

    <script>
        const consoleOutput = document.getElementById('console-output');
        const originalConsole = {
            log: console.log,
            error: console.error,
            warn: console.warn,
            info: console.info
        };
        
        function captureConsole() {
            console.log = (...args) => {
                originalConsole.log(...args);
                const line = document.createElement('div');
                line.textContent = '[LOG] ' + args.map(arg => 
                    typeof arg === 'object' ? JSON.stringify(arg) : arg
                ).join(' ');
                consoleOutput.appendChild(line);
                consoleOutput.scrollTop = consoleOutput.scrollHeight;
            };
            
            console.error = (...args) => {
                originalConsole.error(...args);
                const line = document.createElement('div');
                line.style.color = '#f44336';
                line.textContent = '[ERROR] ' + args.map(arg => 
                    typeof arg === 'object' ? JSON.stringify(arg) : arg
                ).join(' ');
                consoleOutput.appendChild(line);
                consoleOutput.scrollTop = consoleOutput.scrollHeight;
            };
        }

        function restoreConsole() {
            console.log = originalConsole.log;
            console.error = originalConsole.error;
            console.warn = originalConsole.warn;
            console.info = originalConsole.info;
        }

        class TestRunner {
            constructor() {
                this.tests = [];
                this.results = [];
                this.testResults = document.getElementById('test-results');
                this.summaryElement = document.getElementById('summary');
            }
            
            addTest(name, fn) {
                this.tests.push({ name, fn });
            }
            
            async run() {
                console.log('Starting test run...');

                this.testResults.innerHTML = '';
                this.results = [];
                captureConsole();
                
                let passed = 0;
                let failed = 0;
                
                for (let i = 0; i < this.tests.length; i++) {
                    const test = this.tests[i];
                    const testElement = document.createElement('div');
                    testElement.className = 'test-item';
                    testElement.innerHTML = \`<strong>\${test.name}</strong><br><span>Running...</span>\`;
                    this.testResults.appendChild(testElement);
                    
                    try {
                        await test.fn();
                        testElement.className += ' test-pass';
                        testElement.innerHTML = \`<strong>✓ \${test.name}</strong>\`;
                        passed++;
                        this.results.push({ name: test.name, passed: true });
                    } catch (error) {
                        testElement.className += ' test-fail';
                        testElement.innerHTML = \`
                            <strong>✗ \${test.name}</strong><br>
                            <div class="test-error">\${error.message}</div>
                        \`;
                        failed++;
                        this.results.push({ 
                            name: test.name, 
                            passed: false, 
                            error: error.message 
                        });
                    }
                }

                this.showSummary(passed, failed);
                restoreConsole();
                
                console.log(\`Tests completed: \${passed} passed, \${failed} failed\`);
            }
            
            showSummary(passed, failed) {
                this.summaryElement.style.display = 'block';
                const total = passed + failed;
                const percentage = total > 0 ? Math.round((passed / total) * 100) : 0;
                
                this.summaryElement.innerHTML = \`
                    <h3>Test Summary</h3>
                    <p>Total tests: \${total}</p>
                    <p><span class="\${passed > 0 ? 'summary-pass' : ''}">✓ Passed: \${passed}</span></p>
                    <p><span class="\${failed > 0 ? 'summary-fail' : ''}">✗ Failed: \${failed}</span></p>
                    <p>Success rate: \${percentage}%</p>
                \`;
            }
            
            reset() {
                this.testResults.innerHTML = '';
                this.summaryElement.style.display = 'none';
                consoleOutput.innerHTML = '';
                console.log('Tests reset');
            }
        }

        const runner = new TestRunner();

        runner.addTest('API should exist', () => {
            if (!window.viewportFontMonitor) {
                throw new Error('viewportFontMonitor not found on window');
            }
        });
        
        runner.addTest('API should have start method', () => {
            if (typeof window.viewportFontMonitor.start !== 'function') {
                throw new Error('start method not found or not a function');
            }
        });
        
        runner.addTest('API should have stop method', () => {
            if (typeof window.viewportFontMonitor.stop !== 'function') {
                throw new Error('stop method not found or not a function');
            }
        });
        
        runner.addTest('API should have getStats method', () => {
            if (typeof window.viewportFontMonitor.getStats !== 'function') {
                throw new Error('getStats method not found or not a function');
            }
        });
        
        runner.addTest('API should have triggerPoll method', () => {
            if (typeof window.viewportFontMonitor.triggerPoll !== 'function') {
                throw new Error('triggerPoll method not found or not a function');
            }
        });

        runner.addTest('Should initialize without errors', async () => {
            return new Promise((resolve, reject) => {
                try {
                    if (window.viewportFontMonitor._monitor) {
                        console.log('Cleaning up existing monitor...');
                        window.viewportFontMonitor.stop();
                    }

                    window.__viewportFontStatisticLoaded = false;
                    if (document.readyState !== 'complete') {
                        document.addEventListener('DOMContentLoaded', () => {
                            resolve();
                        });
                        setTimeout(resolve, 1000);
                    } else {
                        resolve();
                    }
                } catch (error) {
                    reject(new Error('Initialization failed: ' + error.message));
                }
            });
        });
        
        runner.addTest('Should start monitor', async () => {
            return new Promise((resolve, reject) => {
                try {
                    console.log('Attempting to start monitor...');
                    const timeout = setTimeout(() => {
                        reject(new Error('Monitor start timeout (5 seconds)'));
                    }, 5000);

                    const originalReport = window.ArkWebContentChangeDetector.reportContentChange;
                    let gotReport = false;
                    
                    window.ArkWebContentChangeDetector.reportContentChange = function(message) {
                        originalReport(message);
                        if (message.type === 'debugLog') {
                            console.log('Monitor debug:', message.data.log);
                        }
                        if (message.type === 'textChange') {
                            gotReport = true;
                            console.log('Got text change report:', message.data.textChangeRatio);
                        }
                    };

                    window.viewportFontMonitor.start(0.1, 200);

                    setTimeout(() => {
                        clearTimeout(timeout);
                        
                        if (window.viewportFontMonitor._monitor) {
                            console.log('Monitor instance created successfully');
                            resolve();
                        } else {
                            reject(new Error('Monitor instance is null after start'));
                        }
                    }, 1000);
                    
                } catch (error) {
                    reject(new Error('Start failed: ' + error.message));
                }
            });
        });
        
        runner.addTest('Should get basic stats', () => {
            const stats = window.viewportFontMonitor.getStats();
            
            if (!stats) {
                throw new Error('getStats returned null');
            }
            

            if (typeof stats !== 'object') {
                throw new Error('getStats should return an object');
            }
            
            console.log('Stats object:', stats);
            

            if (stats.config) {
                console.log('Config found:', stats.config);
            }
        });
        
        runner.addTest('Should stop monitor', () => {
            try {
                window.viewportFontMonitor.stop();

                if (window.viewportFontMonitor._monitor !== null) {
                    throw new Error('Monitor not properly stopped');
                }
                
                console.log('Monitor stopped successfully');
            } catch (error) {
                throw new Error('Stop failed: ' + error.message);
            }
        });
        
        document.getElementById('run-tests').addEventListener('click', () => {
            console.clear();
            runner.run();
        });
        
        document.getElementById('reset-tests').addEventListener('click', () => {
            runner.reset();
        });

        window.addEventListener('load', () => {
            console.log('Page loaded, ready for testing');

            setTimeout(() => {
                const basicTests = runner.tests.slice(0, 5);
                let passed = 0;
                
                basicTests.forEach((test, index) => {
                    try {
                        test.fn();
                        console.log(\`✓ \${test.name}\`);
                        passed++;
                    } catch (error) {
                        console.log(\`✗ \${test.name}: \${error.message}\`);
                    }
                });
                
                console.log(\`Basic API check: \${passed}/\${basicTests.length} passed\`);
            }, 500);
        });
    </script>

    <script>
        window.ArkWebContentChangeDetector = window.ArkWebContentChangeDetector || {
            reportContentChange: function(message) {
                console.log('[REPORT]', message);
            }
        };
        
        window.__viewportFontStatisticLoaded = false;

        ${scriptContent}
        
        console.log('Original script loaded');
    </script>
</body>
</html>
`;

const outputPath = path.join(__dirname, 'test-runner-complete.html');
fs.writeFileSync(outputPath, htmlTemplate);
console.log(`Test runner created: ${outputPath}`);
console.log('\nInstructions:');
console.log('1. Open the HTML file in a modern web browser');
console.log('2. Click "Run Tests" to execute all tests');
console.log('3. Check the console for detailed output');
console.log('4. Use "Reset Tests" to clear results');
console.log('\nThe test runner will:');
console.log('- Verify basic API existence');
console.log('- Test monitor start/stop functionality');
console.log('- Validate stat collection');
console.log('- Provide visual feedback and debugging info');