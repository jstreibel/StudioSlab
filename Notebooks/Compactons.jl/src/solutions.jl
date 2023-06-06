export γ
export oscillon, ∂ₜoscillon, ∂ₓoscillon
export kink, ∂ₜkink, ∂ₓkink

# Lorentz transformations

γ(V) = 1.0 / √(1 - V^2)

# Oscillon

x_L(t, v₀, l) = v₀ * t
x_R(t, v₀, l) = x_L(t, v₀, l) + l

function F(x, v₀, l)
    if 0 ≤ x ≤ (1 + v₀) * l / 2
        -0.25 / (1.0 + v₀) * x^2
    elseif (1 + v₀) * l / 2 < x ≤ l
        -l^2 / 8.0 + 0.25 / (1 - v₀) * (l - x)^2
    else
        0.0
    end
end

function f(x, v₀, l)
    if 0 ≤ x ≤ (1 + v₀) * l / 2
        -x / (1 + v₀)
    elseif (1 + v₀) * l / 2 < x ≤ l
        -(l - x) / (1 - v₀)
    else
        0.0
    end
end

τ(t, l) = abs(mod(t - l / 2, l) - l / 2)
σ(t, l) = sign(mod(t - l / 2, l) - l / 2)

function oscillon(t, x; v₀=0.0, l=1.0)
    τₗ = τ(t, l)
    σₗ = σ(t, l)

    if (x_L(τₗ, v₀, l) ≤ x ≤ τₗ)
        σₗ * (F(x + τₗ, v₀, l) - F(x - τₗ + l, v₀, l) + τₗ^2 / 2.0 - l^2 / 8.0)
    elseif (τₗ < x ≤ l - τₗ)
        σₗ * (F(x + τₗ, v₀, l) - F(x - τₗ, v₀, l) + τₗ^2 / 2.0)
    elseif (l - τₗ < x ≤ x_R(τₗ, v₀, l))
        σₗ * (F(x + τₗ - l, v₀, l) - F(x - τₗ, v₀, l) + τₗ^2 / 2.0 - l^2 / 8.0)
    else
        0.0
    end
end

function ∂ₜoscillon(t, x; v₀=0.0, l=1.0)
    τₗ = τ(t, l)

    if x_L(τₗ, v₀, l) ≤ x ≤ τₗ
        0.5 * f(x + τₗ, v₀, l) + 0.5 * f(x - τₗ + l, v₀, l) + τₗ
    elseif τₗ < x ≤ l - τₗ
        0.5 * f(x + τₗ, v₀, l) + 0.5 * f(x - τₗ, v₀, l) + τₗ
    elseif l - τₗ < x ≤ x_R(τₗ, v₀, l)
        0.5 * f(x + τₗ - l, v₀, l) + 0.5 * f(x - τₗ, v₀, l) + τₗ
    else
        0.0
    end
end

function ∂ₓoscillon(t, x; v₀=0.0, l=1.0)
    τₗ = τ(t, l)
    σₗ = σ(t, l)

    if x_L(τₗ, v₀, l) ≤ x ≤ τₗ
        σₗ * 0.5 * (f(x + τₗ, v₀, l) - f(x - τₗ + l, v₀, l))
    elseif τₗ < x ≤ l - τₗ
        σₗ * 0.5 * (f(x + τₗ, v₀, l) - f(x - τₗ, v₀, l))
    elseif l - τₗ < x ≤ x_R(τₗ, v₀, l)
        σₗ * 0.5 * (f(x + τₗ - l, v₀, l) - f(x - τₗ, v₀, l))
    else
        0.0
    end
end

# Moving oscillon

function oscillon(t, x, V; v₀=0.0, l=1.0)
    t′ = γ(V) * (t - V * x)
    x′ = γ(V) * (x - V * t)

    oscillon(t′, x′, v₀=v₀, l=l)
end

function ∂ₜoscillon(t, x, V; v₀=0.0, l=1.0)
    t′ = γ(V) * (t - V * x)
    x′ = γ(V) * (x - V * t)

    γ(V) * (∂ₜoscillon(t′, x′; v₀=v₀, l=l) - V * ∂ₓoscillon(t′, x′; v₀=v₀, l=l))
end

function ∂ₓoscillon(t, x, V; v₀=0.0, l=1.0)
    t′ = γ(V) * (t - V * x)
    x′ = γ(V) * (x - V * t)

    γ(V) * (∂ₓoscillon(t′, x′; v₀=v₀, l=l) - V * ∂ₜoscillon(t′, x′; v₀=v₀, l=l))
end

# Kink

function kink(x)
    if x ≤ 0
        0.0
    elseif x ≤ π
        1 - cos(x)
    else
        2.0
    end
end

kink(t, x) = kink(x)

function ∂ₓkink(x)
    if 0 ≤ x ≤ π
        sin(x)
    else
        0.0
    end
end

∂ₓkink(t, x) = ∂ₓkink(x)

# Moving kink

function kink(t, x, V)
    x′ = γ(V) * (x - V * t)

    kink(x′)
end

function ∂ₜkink(t, x, V)
    x′ = γ(V) * (x - V * t)

    -V * γ(V) * ∂ₓkink(x′)
end


function ∂ₓkink(t, x, V)
    x′ = γ(V) * (x - V * t)

    γ(V) * ∂ₓkink(x′)
end
