For quest+, the psychometric function knows at compile time the stimulus domain (i.e. the continuous independent vars), the parameter domain (e.g. threshold, slope, bounds), the outcome domain (which is always 1D, and is generally {0, 1} (more for multinomial))

So inheritance-wise, we have the super-generic Base for `update` and `next`

```c++
template <typename T = double>
class Base
{
    public:
        virtual T next() = 0;
        virtual bool update(int response, std::optional<T> intensity) = 0;
};
```

The base quest+ psychometric function

```c++

template <std::size_t DimParam, std::size_t DimStim, std::size_t NResp = 2>
class PsychometricFunction
{
    public:
        virtual xt::xtensor<double, DimParam> generate_prior() = 0;
        // +1 for response dimension
        virtual xt::xtensor<double, DimParam+DimStim+1> generate_likelihoods() = 0;
    private:
        static constexpr std::size_t dim_param = DimParam;
        static constexpr std::size_t dim_stim = DimStim;
        static constexpr std::size_t n_resp = NResp;
};
```

(does order of dims matter for performance?)
(be aware of order of parameters in prior, and params/stim/resp order in likelihood)
Derived ones (e.g. normal CDF)

```c++
class NormCDF : public PsychometricFunction<4, 1, 2>
{

};
```
