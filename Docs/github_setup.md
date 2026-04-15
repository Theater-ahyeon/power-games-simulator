# GitHub Repository Setup Guide

## Step 1: Create Repository on GitHub
1. Go to https://github.com/new
2. Repository name: `power-games-simulator`
3. Description: `A deep political strategy game centered on power struggle - Power Game: The Art of Political Intrigue`
4. Set to **Public**
5. Do NOT initialize with README (we already have one)
6. Click "Create repository"

## Step 2: Add Topics/Tags
After creating, go to repository settings and add these topics:
- `political-strategy-game`
- `game-development`
- `decision-making-simulator`
- `political-intrigue`
- `multi-ending-game`
- `cpp`
- `strategy-game`

## Step 3: Push Code
Run these commands in the project directory:

```bash
cd d:\PowerGame
git remote add origin https://github.com/YOUR_USERNAME/power-games-simulator.git
git push -u origin main
git push origin develop
git push origin --tags
```

## Step 4: Set Default Branch
1. Go to Settings > Branches
2. Set default branch to `main`
3. Add branch protection rule for `main` (require pull request before merging)

## Step 5: Configure Repository
- Enable Issues
- Enable Discussions
- Enable Wiki
- Add repository secrets if needed for CI/CD

## Branch Strategy
- `main` - Stable releases only
- `develop` - Active development
- Feature branches: `feature/xxx`
- Bug fix branches: `fix/xxx`
- Release branches: `release/vX.X.X`
